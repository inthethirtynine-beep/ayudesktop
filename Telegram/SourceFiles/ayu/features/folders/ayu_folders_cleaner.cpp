// This is the source code of AyuGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "ayu/features/folders/ayu_folders_cleaner.h"

#include "data/data_chat_filters.h"
#include "data/data_session.h"
#include "data/data_folder.h"
#include "history/history.h"
#include "main/main_session.h"
#include "window/window_session_controller.h"
#include "ui/boxes/confirm_box.h"
#include "lang/lang_keys.h"

namespace Ayu::Folders {
namespace {

bool CleanSingleFilter(
		not_null<Main::Session*> session,
		const Data::ChatFilter &filter) {
	if (!filter.id() || filter.chatlist()) {
		return false;
	}

	auto always = filter.always();
	auto pinned = filter.pinned();
	auto changed = false;

	// Удаляем чаты, находящиеся в архиве, из списка always
	for (auto it = always.begin(); it != always.end();) {
		const auto history = *it;
		if (history->folderKnown() && history->folder()) {
			it = always.erase(it);
			changed = true;
		} else {
			++it;
		}
	}

	// Удаляем чаты, находящиеся в архиве, из списка pinned
	for (auto it = pinned.begin(); it != pinned.end();) {
		const auto history = *it;
		if (history->folderKnown() && history->folder()) {
			it = pinned.erase(it);
			changed = true;
		} else {
			++it;
		}
	}

	// Добавляем флаг NoArchived, если его не было
	auto flags = filter.flags();
	if (!(flags & Data::ChatFilter::Flag::NoArchived)) {
		flags |= Data::ChatFilter::Flag::NoArchived;
		changed = true;
	}

	if (!changed) {
		return false;
	}

	const auto updated = Data::ChatFilter(
		filter.id(),
		filter.title(),
		filter.iconEmoji(),
		filter.colorIndex(),
		flags,
		std::move(always),
		std::move(pinned),
		filter.never());

	const auto tl = updated.tl();
	session->data().chatsFilters().apply(MTP_updateDialogFilter(
		MTP_flags(MTPDupdateDialogFilter::Flag::f_filter),
		MTP_int(filter.id()),
		tl));

	session->api().request(MTPmessages_UpdateDialogFilter(
		MTP_flags(MTPmessages_UpdateDialogFilter::Flag::f_filter),
		MTP_int(filter.id()),
		tl
	)).send();

	return true;
}

} // namespace

void CleanFolderFromArchive(
		not_null<Window::SessionController*> controller,
		FilterId filterId) {
	auto &filters = controller->session().data().chatsFilters();
	const auto i = ranges::find(filters.list(), filterId, &Data::ChatFilter::id);
	if (i == end(filters.list())) {
		return;
	}

	if (CleanSingleFilter(&controller->session(), *i)) {
		controller->showToast(tr::ayu_CleanArchiveFromFolderDone(tr::now));
	} else {
		controller->showToast(tr::ayu_CleanArchiveFromFoldersNone(tr::now));
	}
}

void CleanAllFoldersFromArchive(
		not_null<Window::SessionController*> controller) {
	const auto session = &controller->session();
	auto box = Ui::MakeConfirmBox(Ui::ConfirmBoxArgs{
		.text = tr::ayu_CleanArchiveFromFoldersBoxText(tr::now),
		.confirmed = crl::guard(controller, [=] {
			auto &realFilters = session->data().chatsFilters();
			auto updatedCount = 0;
			for (const auto &filter : realFilters.list()) {
				if (CleanSingleFilter(session, filter)) {
					++updatedCount;
				}
			}
			if (updatedCount > 0) {
				controller->showToast(tr::ayu_CleanArchiveFromFoldersDone(tr::now));
			} else {
				controller->showToast(tr::ayu_CleanArchiveFromFoldersNone(tr::now));
			}
		}),
		.confirmText = tr::ayu_CleanArchiveFromFolders(tr::now),
	});
	controller->show(std::move(box));
}

} // namespace Ayu::Folders
