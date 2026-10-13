// This is the source code of AyuGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#pragma once

#include "data/data_chat_filters.h"

namespace Window {
class SessionController;
} // namespace Window

namespace Ayu::Folders {

// Очистить выбранную папку от всех архивных чатов
void CleanFolderFromArchive(
	not_null<Window::SessionController*> controller,
	FilterId filterId);

// Очистить все папки от всех архивных чатов с подтверждением в диалоговом окне
void CleanAllFoldersFromArchive(
	not_null<Window::SessionController*> controller);

} // namespace Ayu::Folders
