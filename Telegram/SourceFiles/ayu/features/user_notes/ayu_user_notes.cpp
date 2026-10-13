// This is the source code of AyuGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "ayu/features/user_notes/ayu_user_notes.h"

#include "ayu/libs/json.hpp"
#include "base/unixtime.h"
#include "core/application.h"

#include <fstream>

using json = nlohmann::json;

namespace AyuFeatures {
namespace {

std::string getNotesPath() {
	return (cWorkingDir() + u"tdata/ayu_user_notes.json"_q).toStdString();
}

} // namespace

UserNotes::UserNotes() {
	load();
}

UserNotes &UserNotes::getInstance() {
	static UserNotes instance;
	return instance;
}

bool UserNotes::has(PeerId peerId) const {
	const auto it = _notes.find(peerId);
	if (it == _notes.end()) {
		return false;
	}
	return !it->second.tag.trimmed().isEmpty() || !it->second.note.trimmed().isEmpty();
}

UserNote UserNotes::get(PeerId peerId) const {
	const auto it = _notes.find(peerId);
	return (it != _notes.end()) ? it->second : UserNote();
}

QString UserNotes::tag(PeerId peerId) const {
	const auto it = _notes.find(peerId);
	return (it != _notes.end()) ? it->second.tag : QString();
}

QString UserNotes::note(PeerId peerId) const {
	const auto it = _notes.find(peerId);
	return (it != _notes.end()) ? it->second.note : QString();
}

void UserNotes::set(PeerId peerId, const QString &tag, const QString &note) {
	const auto cleanTag = tag.trimmed();
	const auto cleanNote = note.trimmed();
	if (cleanTag.isEmpty() && cleanNote.isEmpty()) {
		remove(peerId);
		return;
	}
	auto &entry = _notes[peerId];
	entry.tag = cleanTag;
	entry.note = cleanNote;
	entry.updatedAt = base::unixtime::now();
	_noteVersions[peerId]++;
	save();
	_noteChanged.fire_copy(peerId);
}

void UserNotes::remove(PeerId peerId) {
	if (_notes.erase(peerId) > 0) {
		_noteVersions[peerId]++;
		save();
		_noteChanged.fire_copy(peerId);
	}
}

int UserNotes::version(PeerId peerId) const {
	const auto it = _noteVersions.find(peerId);
	return (it != _noteVersions.end()) ? it->second : 0;
}

rpl::producer<PeerId> UserNotes::noteChanged() const {
	return _noteChanged.events();
}

void UserNotes::load() {
	std::ifstream file(getNotesPath());
	if (!file.good()) {
		return;
	}
	try {
		json data;
		file >> data;
		file.close();
		_notes.clear();
		if (data.is_object()) {
			for (const auto &[key, val] : data.items()) {
				try {
					const auto id = PeerId(std::stoull(key));
					UserNote note;
					note.tag = QString::fromStdString(val.value("tag", ""));
					note.note = QString::fromStdString(val.value("note", ""));
					note.updatedAt = val.value("updatedAt", uint64(0));
					if (!note.tag.isEmpty() || !note.note.isEmpty()) {
						_notes.emplace(id, std::move(note));
					}
				} catch (...) {
				}
			}
		}
	} catch (...) {
	}
}

void UserNotes::save() {
	json data = json::object();
	for (const auto &[peerId, note] : _notes) {
		data[std::to_string(peerId.value)] = {
			{"tag", note.tag.toStdString()},
			{"note", note.note.toStdString()},
			{"updatedAt", note.updatedAt},
		};
	}
	std::ofstream file(getNotesPath());
	if (file.good()) {
		file << data.dump(4);
		file.close();
	}
}

} // namespace AyuFeatures
