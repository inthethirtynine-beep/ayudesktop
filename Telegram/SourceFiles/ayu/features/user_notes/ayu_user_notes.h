// This is the source code of AyuGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#pragma once

#include "base/flat_map.h"
#include "rpl/producer.h"
#include "rpl/event_stream.h"

#include <QtCore/QString>

class PeerData;

namespace AyuFeatures {

struct UserNote {
	QString tag;
	QString note;
	uint64 updatedAt = 0;
};

class UserNotes final {
public:
	static UserNotes &getInstance();

	[[nodiscard]] bool has(PeerId peerId) const;
	[[nodiscard]] UserNote get(PeerId peerId) const;
	[[nodiscard]] QString tag(PeerId peerId) const;
	[[nodiscard]] QString note(PeerId peerId) const;

	void set(PeerId peerId, const QString &tag, const QString &note);
	void remove(PeerId peerId);

	[[nodiscard]] int version(PeerId peerId) const;

	[[nodiscard]] rpl::producer<PeerId> noteChanged() const;

	void load();
	void save();

private:
	UserNotes();
	~UserNotes() = default;

	base::flat_map<PeerId, UserNote> _notes;
	base::flat_map<PeerId, int> _noteVersions;
	rpl::event_stream<PeerId> _noteChanged;
};

} // namespace AyuFeatures
