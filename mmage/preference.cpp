#include "preference.h"
#include "../resource/resource.h"

#include <shlobj.h>

#define INIFILE				TEXT("mmage.ini")
#define INIFILE_SECTION		TEXT("MMAGE")
#define INIFILE_KEY_VIEWSIZE		TEXT("VIEWSIZE")
#define INIFILE_KEY_HORIZONTAL		TEXT("HORIZONTAL")
#define INIFILE_KEY_QUERYARG		TEXT("QUERYARG")
#define INIFILE_KEY_ARGUMENT		TEXT("ARGUMENT")
#define INIFILE_KEY_ROM0DIR			TEXT("ROM0DIR")
#define INIFILE_KEY_RAM0DIR			TEXT("RAM0DIR")
#define INIFILE_KEY_OWNER_NAME		TEXT("OWNER_NAME")
#define INIFILE_KEY_OWNER_BYEAR		TEXT("OWNER_BYEAR")
#define INIFILE_KEY_OWNER_BMONTH	TEXT("OWNER_BMONTH")
#define INIFILE_KEY_OWNER_BDAY		TEXT("OWNER_BDAY")
#define INIFILE_KEY_OWNER_SEX		TEXT("OWNER_SEX")
#define INIFILE_KEY_OWNER_BLOOD		TEXT("OWNER_BLOOD")

static MMAGE_CONFIG mmconfig_struct;
LPMMAGE_CONFIG mmconfig = &mmconfig_struct;

static LRESULT CALLBACK RunningEnvDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK FileSystemDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK OwnerInfoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

static BOOL SelectFolder(HWND hWnd, LPCTSTR title, LPTSTR path);

#define NUM_DLG 3

static LPCTSTR dlgtmp[NUM_DLG] = {
	(LPCTSTR)IDD_RUNNINGENV,
	(LPCTSTR)IDD_FILESYS,
	(LPCTSTR)IDD_OWNERINFO,
};

static DLGPROC dlgproc[NUM_DLG] = {
	(DLGPROC)RunningEnvDlgProc,
	(DLGPROC)FileSystemDlgProc,
	(DLGPROC)OwnerInfoDlgProc,
};

void OpenPreferenceDlg() {
    PROPSHEETPAGE psp;
    PROPSHEETHEADER psh;
    HPROPSHEETPAGE hpsp[NUM_DLG];
	int i;

    psp.dwSize = sizeof(psp);
    psp.dwFlags = PSP_DEFAULT;
    psp.hInstance = wonw32ctx->hInst;

	for(i = 0; i < NUM_DLG; i++) {
		psp.pszTemplate = dlgtmp[i];
		psp.pfnDlgProc = dlgproc[i];
		hpsp[i] = CreatePropertySheetPage(&psp);
	}

    memset(&psh, 0, sizeof(psh));
    psh.dwSize = sizeof(psh);
    psh.dwFlags = PSH_NOAPPLYNOW;
    psh.hInstance = wonw32ctx->hInst;
    psh.hwndParent = wonw32ctx->hWnd;
    psh.nPages = NUM_DLG;
    psh.phpage = hpsp;
    psh.pszCaption = TEXT("設定");
    PropertySheet(&psh);
    return;
}

static LRESULT CALLBACK RunningEnvDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	int wmId, wmEvent;
	LPNMHDR nmhdr;
	BOOL handled = TRUE;

    switch(message) {
        case WM_INITDIALOG:
			CheckDlgButton(hDlg, IDC_QUERYARG, mmconfig->queryarg);
			EnableWindow(GetDlgItem(hDlg, IDC_ARGUMENT), !mmconfig->queryarg);
			SetWindowText(GetDlgItem(hDlg, IDC_ARGUMENT), mmconfig->argument);
			break;

		case WM_COMMAND:
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			switch(wmId) {
				case IDC_QUERYARG:
					EnableWindow(GetDlgItem(hDlg, IDC_ARGUMENT), !IsDlgButtonChecked(hDlg, IDC_QUERYARG));
					break;

				default:
					handled = FALSE;
					break;
			}
			break;

        case WM_NOTIFY:
            nmhdr = (NMHDR *)lParam;
            switch(nmhdr->code) {
                case PSN_APPLY:
					mmconfig->queryarg = IsDlgButtonChecked(hDlg, IDC_QUERYARG);
					GetWindowText(GetDlgItem(hDlg, IDC_ARGUMENT), mmconfig->argument, MAX_WW_ARG);
					break;
                default:
                    handled = FALSE;
					break;
            }
            break;

		default:
			handled = FALSE;
			break;
    }

    return handled;
}

static LRESULT CALLBACK FileSystemDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	int wmId, wmEvent;
	LPNMHDR nmhdr;
	TCHAR dirname[MAX_PATH + 1];
	BOOL specifydir;
	BOOL handled = TRUE;

    switch(message) {
        case WM_INITDIALOG:
			specifydir = (lstrlen(mmconfig->rom0dir) && lstrlen(mmconfig->ram0dir)) ? TRUE : FALSE;
			CheckDlgButton(hDlg, IDC_SPECIFYDIR, specifydir);
			if(specifydir) {
				SetWindowText(GetDlgItem(hDlg, IDC_ROM0), mmconfig->rom0dir);
				SetWindowText(GetDlgItem(hDlg, IDC_RAM0), mmconfig->ram0dir);
			}
			EnableWindow(GetDlgItem(hDlg, IDC_ARGUMENT), specifydir);
			EnableWindow(GetDlgItem(hDlg, IDC_ROM0), specifydir);
			EnableWindow(GetDlgItem(hDlg, IDC_ROM0REF), specifydir);
			EnableWindow(GetDlgItem(hDlg, IDC_RAM0), specifydir);
			EnableWindow(GetDlgItem(hDlg, IDC_RAM0REF), specifydir);
			break;

		case WM_COMMAND:
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			switch(wmId) {
				case IDC_SPECIFYDIR:
					EnableWindow(GetDlgItem(hDlg, IDC_ROM0), IsDlgButtonChecked(hDlg, IDC_SPECIFYDIR));
					EnableWindow(GetDlgItem(hDlg, IDC_ROM0REF), IsDlgButtonChecked(hDlg, IDC_SPECIFYDIR));
					EnableWindow(GetDlgItem(hDlg, IDC_RAM0), IsDlgButtonChecked(hDlg, IDC_SPECIFYDIR));
					EnableWindow(GetDlgItem(hDlg, IDC_RAM0REF), IsDlgButtonChecked(hDlg, IDC_SPECIFYDIR));
					break;

				case IDC_ROM0REF:
					if(SelectFolder(hDlg, TEXT("/rom0 ディレクトリの指定"), dirname))
						SetWindowText(GetDlgItem(hDlg, IDC_ROM0), dirname);
					break;
				case IDC_RAM0REF:
					if(SelectFolder(hDlg, TEXT("/ram0 ディレクトリの指定"), dirname))
						SetWindowText(GetDlgItem(hDlg, IDC_RAM0), dirname);
					break;

				default:
					handled = FALSE;
					break;
			}
			break;

        case WM_NOTIFY:
            nmhdr = (NMHDR *)lParam;
            switch(nmhdr->code) {
                case PSN_APPLY:
					specifydir = IsDlgButtonChecked(hDlg, IDC_SPECIFYDIR);
					if(specifydir) {
						if(GetWindowTextLength(GetDlgItem(hDlg, IDC_ROM0))
							&& GetWindowTextLength(GetDlgItem(hDlg, IDC_RAM0))) {
							GetWindowText(GetDlgItem(hDlg, IDC_ROM0), mmconfig->rom0dir, MAX_PATH);
							GetWindowText(GetDlgItem(hDlg, IDC_RAM0), mmconfig->ram0dir, MAX_PATH);
						} else {
							MessageBox(hDlg, TEXT("/rom0、/ram0 のマッピングディレクトリを指定してください。"), NULL, MB_OK);
							SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE); 
							break;
						}
					} else {
						lstrcpy(mmconfig->rom0dir, TEXT(""));
						lstrcpy(mmconfig->ram0dir, TEXT(""));
					}
					break;
                default:
                    handled = FALSE;
					break;
            }
            break;

		default:
			handled = FALSE;
			break;
    }

    return handled;
}

static LRESULT CALLBACK OwnerInfoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	LPNMHDR nmhdr;
	SYSTEMTIME day;
	BOOL handled = TRUE;

    switch(message) {
        case WM_INITDIALOG:
			SetWindowText(GetDlgItem(hDlg, IDC_NAME), mmconfig->ownerinfo.name);

			ZeroMemory(&day, sizeof(day));
			day.wYear = mmconfig->ownerinfo.birth_year;
			day.wMonth = mmconfig->ownerinfo.birth_month;
			day.wDay = mmconfig->ownerinfo.birth_day;
			SendMessage(GetDlgItem(hDlg, IDC_BIRTHDAY), DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&day);

			switch(mmconfig->ownerinfo.sex) {
				case 0:
					CheckDlgButton(hDlg, IDC_UNKNOWNSEX, TRUE);
					break;
				case 1:
					CheckDlgButton(hDlg, IDC_MALE, TRUE);
					break;
				case 2:
					CheckDlgButton(hDlg, IDC_FEMALE, TRUE);
					break;
				default:
					CheckDlgButton(hDlg, IDC_UNKNOWNSEX, TRUE);
					break;
			}

			switch(mmconfig->ownerinfo.bloodtype) {
				case 0:
					CheckDlgButton(hDlg, IDC_UNKNOWNBLOOD, TRUE);
					break;
				case 1:
					CheckDlgButton(hDlg, IDC_BLOODA, TRUE);
					break;
				case 2:
					CheckDlgButton(hDlg, IDC_BLOODB, TRUE);
					break;
				case 3:
					CheckDlgButton(hDlg, IDC_BLOODO, TRUE);
					break;
				case 4:
					CheckDlgButton(hDlg, IDC_BLOODAB, TRUE);
					break;
				default:
					CheckDlgButton(hDlg, IDC_UNKNOWNBLOOD, TRUE);
					break;
			}

			break;

        case WM_NOTIFY:
            nmhdr = (NMHDR *)lParam;
            switch(nmhdr->code) {
                case PSN_APPLY:
					GetWindowText(GetDlgItem(hDlg, IDC_NAME), mmconfig->ownerinfo.name, 16);

					SendMessage(GetDlgItem(hDlg, IDC_BIRTHDAY), DTM_GETSYSTEMTIME, 0, (LPARAM)&day);
					mmconfig->ownerinfo.birth_year = day.wYear;
					mmconfig->ownerinfo.birth_month = (char)day.wMonth;
					mmconfig->ownerinfo.birth_day = (char)day.wDay;

					if(IsDlgButtonChecked(hDlg, IDC_UNKNOWNSEX))
						mmconfig->ownerinfo.sex = 0;
					else if(IsDlgButtonChecked(hDlg, IDC_MALE))
						mmconfig->ownerinfo.sex = 1;
					else if(IsDlgButtonChecked(hDlg, IDC_FEMALE))
						mmconfig->ownerinfo.sex = 2;
					
					if(IsDlgButtonChecked(hDlg, IDC_UNKNOWNBLOOD))
						mmconfig->ownerinfo.bloodtype = 0;
					else if(IsDlgButtonChecked(hDlg, IDC_BLOODA))
						mmconfig->ownerinfo.bloodtype = 1;
					else if(IsDlgButtonChecked(hDlg, IDC_BLOODB))
						mmconfig->ownerinfo.bloodtype = 2;
					else if(IsDlgButtonChecked(hDlg, IDC_BLOODO))
						mmconfig->ownerinfo.bloodtype = 3;
					else if(IsDlgButtonChecked(hDlg, IDC_BLOODAB))
						mmconfig->ownerinfo.bloodtype = 4;

                    break;

                default:
                    handled = FALSE;
					break;
            }
            break;

		default:
			handled = FALSE;
			break;
    }

    return handled;
}

static BOOL SelectFolder(HWND hWnd, LPCTSTR title, LPTSTR path) {
    BROWSEINFO  binfo;
    LPITEMIDLIST idlist;

    binfo.hwndOwner = hWnd;
    binfo.pidlRoot = NULL;
    binfo.pszDisplayName = path;
    binfo.lpszTitle = title;
    binfo.ulFlags = BIF_RETURNONLYFSDIRS; 
    binfo.lpfn = NULL;
    binfo.lParam = NULL;          
    binfo.iImage = (int)NULL;

    idlist = SHBrowseForFolder(&binfo);
    if (idlist) {
        SHGetPathFromIDList(idlist,path);
        CoTaskMemFree(idlist);
        return TRUE;
    }
    return FALSE;
}

void LoadConfig() {
	TCHAR inifile[MAX_PATH + 1];

	wsprintf(inifile, TEXT("%s\\%s"), mmconfig->workingdir, INIFILE);

	mmconfig->viewsize = GetPrivateProfileInt(INIFILE_SECTION, INIFILE_KEY_VIEWSIZE, 1, inifile);
	mmconfig->horizontal = GetPrivateProfileInt(INIFILE_SECTION, INIFILE_KEY_HORIZONTAL, MMCONFIG_HORIZONTAL, inifile);
	mmconfig->queryarg = GetPrivateProfileInt(INIFILE_SECTION, INIFILE_KEY_QUERYARG, TRUE, inifile);
	GetPrivateProfileString(INIFILE_SECTION, INIFILE_KEY_ROM0DIR, TEXT(""), mmconfig->rom0dir, MAX_PATH, inifile);
	GetPrivateProfileString(INIFILE_SECTION, INIFILE_KEY_RAM0DIR, TEXT(""), mmconfig->ram0dir, MAX_PATH, inifile);
	GetPrivateProfileString(INIFILE_SECTION, INIFILE_KEY_OWNER_NAME, TEXT(""), mmconfig->ownerinfo.name, 16, inifile);
	mmconfig->ownerinfo.birth_year = GetPrivateProfileInt(INIFILE_SECTION, INIFILE_KEY_OWNER_BYEAR, 1970, inifile);
	mmconfig->ownerinfo.birth_month = GetPrivateProfileInt(INIFILE_SECTION, INIFILE_KEY_OWNER_BMONTH, 1, inifile);
	mmconfig->ownerinfo.birth_day = GetPrivateProfileInt(INIFILE_SECTION, INIFILE_KEY_OWNER_BDAY, 1, inifile);
	mmconfig->ownerinfo.sex = GetPrivateProfileInt(INIFILE_SECTION, INIFILE_KEY_OWNER_SEX, 0, inifile);
	mmconfig->ownerinfo.bloodtype = GetPrivateProfileInt(INIFILE_SECTION, INIFILE_KEY_OWNER_BLOOD, 0, inifile);
}

void SaveConfig() {
	TCHAR inifile[MAX_PATH + 1];
	TCHAR tmp[5];

	wsprintf(inifile, TEXT("%s\\%s"), mmconfig->workingdir, INIFILE);

	wsprintf(tmp, TEXT("%1d"), mmconfig->viewsize);
	WritePrivateProfileString(INIFILE_SECTION, INIFILE_KEY_VIEWSIZE, tmp, inifile);
	wsprintf(tmp, TEXT("%1d"), mmconfig->horizontal);
	WritePrivateProfileString(INIFILE_SECTION, INIFILE_KEY_HORIZONTAL, tmp, inifile);
	wsprintf(tmp, TEXT("%1d"), mmconfig->queryarg);
	WritePrivateProfileString(INIFILE_SECTION, INIFILE_KEY_QUERYARG, tmp, inifile);
	WritePrivateProfileString(INIFILE_SECTION, INIFILE_KEY_ROM0DIR, mmconfig->rom0dir, inifile);
	WritePrivateProfileString(INIFILE_SECTION, INIFILE_KEY_RAM0DIR, mmconfig->ram0dir, inifile);
	WritePrivateProfileString(INIFILE_SECTION, INIFILE_KEY_OWNER_NAME, mmconfig->ownerinfo.name, inifile);
	wsprintf(tmp, TEXT("%4d"), mmconfig->ownerinfo.birth_year);
	WritePrivateProfileString(INIFILE_SECTION, INIFILE_KEY_OWNER_BYEAR, tmp, inifile);
	wsprintf(tmp, TEXT("%2d"), mmconfig->ownerinfo.birth_month);
	WritePrivateProfileString(INIFILE_SECTION, INIFILE_KEY_OWNER_BMONTH, tmp, inifile);
	wsprintf(tmp, TEXT("%2d"), mmconfig->ownerinfo.birth_day);
	WritePrivateProfileString(INIFILE_SECTION, INIFILE_KEY_OWNER_BDAY, tmp, inifile);
	wsprintf(tmp, TEXT("%1d"), mmconfig->ownerinfo.sex);
	WritePrivateProfileString(INIFILE_SECTION, INIFILE_KEY_OWNER_SEX, tmp, inifile);
	wsprintf(tmp, TEXT("%1d"), mmconfig->ownerinfo.bloodtype);
	WritePrivateProfileString(INIFILE_SECTION, INIFILE_KEY_OWNER_BLOOD, tmp, inifile);
}
