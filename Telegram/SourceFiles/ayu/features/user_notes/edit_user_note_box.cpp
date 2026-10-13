// This is the source code of AyuGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "ayu/features/user_notes/edit_user_note_box.h"

#include "ayu/features/user_notes/ayu_user_notes.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "data/data_changes.h"
#include "history/history.h"
#include "lang/lang_keys.h"
#include "styles/style_boxes.h"
#include "styles/style_layers.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/labels.h"
#include "window/window_session_controller.h"

namespace AyuFeatures {

void EditUserNoteBox(
		not_null<Ui::GenericBox*> box,
		not_null<Window::SessionController*> controller,
		not_null<PeerData*> peer) {
	box->setWidth(st::boxWideWidth);
	box->setTitle(rpl::single(u"Локальная заметка и тег"_q));

	const auto currentNote = UserNotes::getInstance().get(peer->id);
	const auto hasNote = UserNotes::getInstance().has(peer->id);

	box->addRow(
		object_ptr<Ui::FlatLabel>(
			box,
			u"Чат: %1"_q.arg(peer->name()),
			st::boxLabel));

	box->addRow(
		object_ptr<Ui::FlatLabel>(
			box,
			u"Заметка и тег хранятся локально на этом устройстве и видны только вам."_q,
			st::boxLabel));

	Ui::AddSkip(box->verticalLayout());

	const auto tagField = box->addRow(
		object_ptr<Ui::InputField>(
			box,
			st::defaultInputField,
			rpl::single(u"Тег / метка (напр. VIP, Клиент, Работа)"_q),
			currentNote.tag));
	tagField->setMaxLength(30);

	Ui::AddSkip(box->verticalLayout());

	const auto noteField = box->addRow(
		object_ptr<Ui::InputField>(
			box,
			st::defaultInputField,
			Ui::InputField::Mode::MultiLine,
			rpl::single(u"Текст заметки (необязательно)"_q),
			currentNote.note));

	box->addButton(tr::lng_settings_save(), [=] {
		const auto tag = tagField->getLastText().trimmed();
		const auto note = noteField->getLastText().trimmed();
		UserNotes::getInstance().set(peer->id, tag, note);

		peer->session().changes().peerUpdated(peer, Data::PeerUpdate::Flag::Name);
		if (const auto history = peer->owner().historyLoaded(peer)) {
			history->updateChatListEntry();
		}
		controller->showToast(u"Заметка сохранена"_q);
		box->closeBox();
	});

	if (hasNote) {
		box->addLeftButton(u"Удалить"_q, [=] {
			UserNotes::getInstance().remove(peer->id);

			peer->session().changes().peerUpdated(peer, Data::PeerUpdate::Flag::Name);
			if (const auto history = peer->owner().historyLoaded(peer)) {
				history->updateChatListEntry();
			}
			controller->showToast(u"Заметка удалена"_q);
			box->closeBox();
		});
	}

	box->addButton(tr::lng_cancel(), [=] {
		box->closeBox();
	});

	box->setFocusCallback([=] {
		tagField->setFocusFast();
	});
}

} // namespace AyuFeatures
