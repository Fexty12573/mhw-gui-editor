#include "pch.h"
#include "GUIEditor.h"
#include "HrException.h"

#include <ShObjIdl.h>
#include <wrl.h>

GUIEditor::GUIEditor() {
    HR_INIT(S_OK);
    HR_ASSERT(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
}

void GUIEditor::render() {
}

void GUIEditor::open_file() {
    HR_INIT(S_OK);

    Microsoft::WRL::ComPtr<IFileOpenDialog> dialog;
    HR_ASSERT(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&dialog)));

    constexpr COMDLG_FILTERSPEC filters[] = {
        { L"MHW GUI Files", L"*.gui" }
    };

    HR_ASSERT(dialog->SetFileTypes(ARRAYSIZE(filters), filters));
    HR_ASSERT(dialog->SetDefaultExtension(L"gui"));
    HR_ASSERT(dialog->SetOptions(
        FOS_STRICTFILETYPES | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST
    ));

    HR_ASSERT(dialog->Show(nullptr));

    Microsoft::WRL::ComPtr<IShellItem> item;
    HR_ASSERT(dialog->GetResult(&item));
    
    LPWSTR file_name;
    HR_ASSERT(item->GetDisplayName(SIGDN_FILESYSPATH, &file_name));

    std::wstring wpath = file_name;
    BinaryReader reader{ { wpath.begin(), wpath.end() } };
    m_file.load_from(reader);
}
