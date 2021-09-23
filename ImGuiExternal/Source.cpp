#include "Source.hpp"

LPCSTR TargetProcess = "JumpKing.exe";
bool ShowMenu = true;
bool ImGui_Initialised = false;
bool CreateConsole = true;

Memory::External memorySpace = Memory::External("JumpKing.exe");

GameData	g_data{ (uintptr_t)0, (uintptr_t)0, 0x08, 0 };

Entity	king;

float	pos[2];

namespace Process {
	DWORD ID;
	HANDLE Handle;
	HWND Hwnd;
	WNDPROC WndProc;
	int WindowWidth;
	int WindowHeight;
	int WindowLeft;
	int WindowRight;
	int WindowTop;
	int WindowBottom;
	LPCSTR Title;
	LPCSTR ClassName;
	LPCSTR Path;
}

namespace OverlayWindow {
	WNDCLASSEX WindowClass;
	HWND Hwnd;
	LPCSTR Name;
}

namespace DirectX9Interface {
	IDirect3D9Ex* Direct3D9 = NULL;
	IDirect3DDevice9Ex* pDevice = NULL;
	D3DPRESENT_PARAMETERS pParams = { NULL };
	MARGINS Margin = { -1 };
	MSG Message = { NULL };
}

void InputHandler() {
	for (int i = 0; i < 5; i++) ImGui::GetIO().MouseDown[i] = false;
	int button = -1;
	if (GetAsyncKeyState(VK_LBUTTON)) button = 0;
	if (button != -1) ImGui::GetIO().MouseDown[button] = true;
}

void Draw() {
	char FpsInfo[64];
	sprintf(FpsInfo, "Overlay FPS: %0.f", ImGui::GetIO().Framerate);
	RGBA White = { 255,255,255,255 };
	DrawStrokeText(30, 44, &White, FpsInfo);
}

void	Entity::display(void) const
{
	//ImGui::Text("%p : %d/%d", addr, hp, hp_max);

	//ImGui::Text("  %d x : %.1f y : %.1f z : %.1f", gd, x, y, z);
	//ImGui::Text("  ax : %p ay : %p az : %p", a_x, a_y, a_z);

	//std::cout << "pos : " << x << " : " << y << " : " << z << std::endl;
	//std::cout << "a   : " << a_x << " : " << a_y << " : " << a_z << std::endl;
}

void	panel_menu(GameData& data)
{
	static float f0 = king.x;
	static float f1 = king.y;
	static float f2 = king.vx;
	static float f3 = king.vy;

	ImGui::Begin("King's Checkpoint");

	/// Save / Load buttons
	ImGui::Text("Check point "); ImGui::SameLine();
	if (ImGui::Button("save"))
	{
		pos[0] = memorySpace.read<float>(king.a_x);
		pos[1] = memorySpace.read<float>(king.a_y);
		std::cout << "position saved" << std::endl;
	}
	ImGui::SameLine();
	if (ImGui::Button("load"))
	{

		//float step[2];

		//float xx = king.x - pos[0];
		//float yy = pos[0] - king.x;
		//step[0] = (xx > 0) - (xx < 0);	//(pos[0] - king.x) / 20;
		////step[1] = (pos[1] - king.y) / 20;
		//step[1] = (yy > 0) - (yy < 0);

		//memorySpace.write<float>(king.a_vx, 0);
		//memorySpace.write<float>(king.a_vx, 0);
		//memorySpace.write<float>(king.a_x, pos[0]);
		//memorySpace.write<float>(king.a_y, pos[1]);

		//while (king.x != pos[0] && king.y != pos[1])
		////for (int ii = 0; ii < 20; ++ii)
		//{
		//	//std::cout << "oui" << std::endl;
		//	memorySpace.write<float>(king.a_vx, step[0]);
		//	memorySpace.write<float>(king.a_vy, step[1]);

		//	memorySpace.write<float>(king.a_x, king.x + step[0]);
		//	memorySpace.write<float>(king.a_y, king.y + step[1]);
		//	//memorySpace.write<float>(king.a_y, pos[1]);

		//	//Sleep(100);
		//}
		memorySpace.write<float>(king.a_vx, 0);
		memorySpace.write<float>(king.a_vy, 0);
		memorySpace.write<float>(king.a_x, pos[0]);
		memorySpace.write<float>(king.a_y, pos[1]);
		std::cout << "position loaded" << std::endl;
	}
	ImGui::Separator();

	/// drag sliders
	ImGui::Text("Manualy change Position");
	ImGui::DragFloat("X", &f0, 0.5f);
	ImGui::SameLine();
	if (ImGui::Button("New X"))
	{
		memorySpace.write<float>(king.a_vx, 0);
		memorySpace.write<float>(king.a_vy, 0);
		memorySpace.write<float>(king.a_x, f0);
	}
	ImGui::DragFloat("Y", &f1, 1.f);
	ImGui::SameLine();
	if (ImGui::Button("New Y"))
	{
		memorySpace.write<float>(king.a_vx, 0);
		memorySpace.write<float>(king.a_vy, 0);
		memorySpace.write<float>(king.a_y, f1);
	}
	if (ImGui::Button("New X && New Y"))
	{
		memorySpace.write<float>(king.a_vx, 0);
		memorySpace.write<float>(king.a_vy, 0);
		memorySpace.write<float>(king.a_x, f0);
		memorySpace.write<float>(king.a_y, f1);
	}
	ImGui::Separator();
	ImGui::Text("Manualy change Velocity");
	ImGui::DragFloat("VX", &f2, 0.1f);
	ImGui::SameLine();
	if (ImGui::Button("New VX"))
	{
		memorySpace.write<float>(king.a_vx, f2);
	}
	ImGui::DragFloat("VY", &f3, 0.1f);
	ImGui::SameLine();
	if (ImGui::Button("New VY"))
	{
		memorySpace.write<float>(king.a_vy, f3);
	}
	if (ImGui::Button("New VX && New VY"))
	{
		memorySpace.write<float>(king.a_vx, f2);
		memorySpace.write<float>(king.a_vy, f3);
	}

	/// debug
	ImGui::Separator();
	//ImGui::Text("baseAddr : %p", data.baseAddr);
	//ImGui::Text("a_x : %p", king.a_x);
	ImGui::Text("x   : %.0f", king.x);
	ImGui::Text("y   : %.0f", king.y);
	ImGui::Text("(%.0f, ", king.vx); ImGui::SameLine();
	ImGui::Text(" %.0f)", king.vy);

}

void Render() {
	if (GetAsyncKeyState(VK_INSERT) & 1) ShowMenu = !ShowMenu;
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//Draw();
	ImGui::GetIO().MouseDrawCursor = ShowMenu;

	if (ShowMenu == true) {
		InputHandler();
		//ImGui::ShowDemoWindow();

		panel_menu(g_data);

		/*SetWindowLong(OverlayWindow::Hwnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
		UpdateWindow(OverlayWindow::Hwnd);*/
		SetWindowLong(OverlayWindow::Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
		UpdateWindow(OverlayWindow::Hwnd);
		SetFocus(OverlayWindow::Hwnd);
	}
	else {
		SetWindowLong(OverlayWindow::Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
		UpdateWindow(OverlayWindow::Hwnd);
	}
	ImGui::EndFrame();

	DirectX9Interface::pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (DirectX9Interface::pDevice->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		DirectX9Interface::pDevice->EndScene();
	}

	HRESULT result = DirectX9Interface::pDevice->Present(NULL, NULL, NULL, NULL);
	if (result == D3DERR_DEVICELOST && DirectX9Interface::pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
		ImGui_ImplDX9_InvalidateDeviceObjects();
		DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}

void MainLoop() {
	static RECT OldRect;
	ZeroMemory(&DirectX9Interface::Message, sizeof(MSG));

	while (DirectX9Interface::Message.message != WM_QUIT) {
		if (PeekMessage(&DirectX9Interface::Message, OverlayWindow::Hwnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&DirectX9Interface::Message);
			DispatchMessage(&DirectX9Interface::Message);
		}
		HWND ForegroundWindow = GetForegroundWindow();
		if (ForegroundWindow == Process::Hwnd) {
			HWND TempProcessHwnd = GetWindow(ForegroundWindow, GW_HWNDPREV);
			SetWindowPos(OverlayWindow::Hwnd, TempProcessHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		RECT TempRect;
		POINT TempPoint;
		ZeroMemory(&TempRect, sizeof(RECT));
		ZeroMemory(&TempPoint, sizeof(POINT));

		GetClientRect(Process::Hwnd, &TempRect);
		ClientToScreen(Process::Hwnd, &TempPoint);

		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = Process::Hwnd;

		if (TempRect.left != OldRect.left || TempRect.right != OldRect.right || TempRect.top != OldRect.top || TempRect.bottom != OldRect.bottom) {
			OldRect = TempRect;
			Process::WindowWidth = TempRect.right;
			Process::WindowHeight = TempRect.bottom;
			DirectX9Interface::pParams.BackBufferWidth = Process::WindowWidth;
			DirectX9Interface::pParams.BackBufferHeight = Process::WindowHeight;
			SetWindowPos(OverlayWindow::Hwnd, (HWND)0, TempPoint.x, TempPoint.y, Process::WindowWidth, Process::WindowHeight, SWP_NOREDRAW);
			DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
		}
		Render();
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (DirectX9Interface::pDevice != NULL) {
		DirectX9Interface::pDevice->EndScene();
		DirectX9Interface::pDevice->Release();
	}
	if (DirectX9Interface::Direct3D9 != NULL) {
		DirectX9Interface::Direct3D9->Release();
	}
	DestroyWindow(OverlayWindow::Hwnd);
	UnregisterClass(OverlayWindow::WindowClass.lpszClassName, OverlayWindow::WindowClass.hInstance);
}

bool DirectXInit() {
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX9Interface::Direct3D9))) {
		return false;
	}

	D3DPRESENT_PARAMETERS Params = { 0 };
	Params.Windowed = TRUE;
	Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Params.hDeviceWindow = OverlayWindow::Hwnd;
	Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	Params.BackBufferWidth = Process::WindowWidth;
	Params.BackBufferHeight = Process::WindowHeight;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.EnableAutoDepthStencil = TRUE;
	Params.AutoDepthStencilFormat = D3DFMT_D16;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	if (FAILED(DirectX9Interface::Direct3D9->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, OverlayWindow::Hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Params, 0, &DirectX9Interface::pDevice))) {
		DirectX9Interface::Direct3D9->Release();
		return false;
	}

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplWin32_Init(OverlayWindow::Hwnd);
	ImGui_ImplDX9_Init(DirectX9Interface::pDevice);
	DirectX9Interface::Direct3D9->Release();
	return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message) {
	case WM_DESTROY:
		if (DirectX9Interface::pDevice != NULL) {
			DirectX9Interface::pDevice->EndScene();
			DirectX9Interface::pDevice->Release();
		}
		if (DirectX9Interface::Direct3D9 != NULL) {
			DirectX9Interface::Direct3D9->Release();
		}
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (DirectX9Interface::pDevice != NULL && wParam != SIZE_MINIMIZED) {
			ImGui_ImplDX9_InvalidateDeviceObjects();
			DirectX9Interface::pParams.BackBufferWidth = LOWORD(lParam);
			DirectX9Interface::pParams.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void SetupWindow() {
	OverlayWindow::WindowClass = {
		sizeof(WNDCLASSEX), 0, WinProc, 0, 0, nullptr, LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, OverlayWindow::Name, LoadIcon(nullptr, IDI_APPLICATION)
	};

	RegisterClassEx(&OverlayWindow::WindowClass);
	if (Process::Hwnd){
		static RECT TempRect = { NULL };
		static POINT TempPoint;
		GetClientRect(Process::Hwnd, &TempRect);
		ClientToScreen(Process::Hwnd, &TempPoint);
		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		Process::WindowWidth = TempRect.right;
		Process::WindowHeight = TempRect.bottom;
	}

	OverlayWindow::Hwnd = CreateWindowEx(NULL, OverlayWindow::Name, OverlayWindow::Name, WS_POPUP | WS_VISIBLE, Process::WindowLeft, Process::WindowTop, Process::WindowWidth, Process::WindowHeight, NULL, NULL, 0, NULL);
	DwmExtendFrameIntoClientArea(OverlayWindow::Hwnd, &DirectX9Interface::Margin);
	SetWindowLong(OverlayWindow::Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	ShowWindow(OverlayWindow::Hwnd, SW_SHOW);
	UpdateWindow(OverlayWindow::Hwnd);
}

DWORD WINAPI	ProcessCheck(LPVOID lpParameter) {
	while (true) {
		if (Process::Hwnd != NULL) {
			if (GetProcessId(TargetProcess) == 0) {
				exit(0);
			}
		}
	}
}
// ###############################################################
//#include <winternl.h>
//#include <processthreadsapi.h>

//typedef struct _CLIENT_ID {
//	HANDLE UniqueProcess;
//	HANDLE UniqueThread;
//} CLIENT_ID;

//returned structure:
//typedef LONG KPRIORITY;
//struct THREAD_BASIC_INFORMATION
//{
//	NTSTATUS                ExitStatus;
//	PVOID                   TebBaseAddress;
//	CLIENT_ID               ClientId;
//	KAFFINITY               AffinityMask;
//	KPRIORITY               Priority;
//	KPRIORITY               BasePriority;
//};
//
//void* GetStackBase(HANDLE hProc, HANDLE hThread)
//{
//	//BOOL menfou = false;
//	THREAD_BASIC_INFORMATION TBI{ 0 };
//
//	NT_TIB TIB;
//	if (!ReadProcessMemory(hProc, TBI.TebBaseAddress, &TIB, sizeof(TIB), nullptr))
//		return nullptr;
//
//	return TIB.StackBase;
//}

// ###############################################################

#include "ntinfo.h"

typedef struct _CLIENT_ID
{
	PVOID UniqueProcess;
	PVOID UniqueThread;
} CLIENT_ID, * PCLIENT_ID;

typedef struct _THREAD_BASIC_INFORMATION
{
	NTSTATUS                ExitStatus;
	PVOID                   TebBaseAddress;
	CLIENT_ID               ClientId;
	KAFFINITY               AffinityMask;
	KPRIORITY               Priority;
	KPRIORITY               BasePriority;
} THREAD_BASIC_INFORMATION, * PTHREAD_BASIC_INFORMATION;

enum THREADINFOCLASS
{
	ThreadBasicInformation,
};

void* GetThreadStackTopAddress_x86(HANDLE hProcess, HANDLE hThread)
{
	LPSTR moduleName = (char *) "ntdll.dll";

	bool loadedManually = false;
	HMODULE module = GetModuleHandle(moduleName);

	if (!module)
	{
		module = LoadLibrary(moduleName);
		loadedManually = true;
	}

	NTSTATUS(__stdcall * NtQueryInformationThread)(HANDLE ThreadHandle, THREADINFOCLASS ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength, PULONG ReturnLength);
	NtQueryInformationThread = reinterpret_cast<decltype(NtQueryInformationThread)>(GetProcAddress(module, "NtQueryInformationThread"));

	if (NtQueryInformationThread)
	{
		NT_TIB tib = { 0 };
		THREAD_BASIC_INFORMATION tbi = { 0 };

		NTSTATUS status = NtQueryInformationThread(hThread, ThreadBasicInformation, &tbi, sizeof(tbi), nullptr);
		if (status >= 0)
		{
			ReadProcessMemory(hProcess, tbi.TebBaseAddress, &tib, sizeof(tbi), nullptr);

			if (loadedManually)
			{
				FreeLibrary(module);
			}
			return tib.StackBase;
		}
	}


	if (loadedManually)
	{
		FreeLibrary(module);
	}

	return nullptr;
}

std::vector<DWORD> threadList(DWORD pid) {
	/* solution from http://stackoverflow.com/questions/1206878/enumerating-threads-in-windows */
	std::vector<DWORD> vect = std::vector<DWORD>();
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (h == INVALID_HANDLE_VALUE)
		return vect;
	THREADENTRY32 te;
	te.dwSize = sizeof(te);
	if (Thread32First(h, &te)) {
		do {
			if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) +
				sizeof(te.th32OwnerProcessID)) {
				if (te.th32OwnerProcessID == pid) {
					//printf("PID: %04d Thread ID: 0x%04x\n", te.th32OwnerProcessID, te.th32ThreadID);
					vect.push_back(te.th32ThreadID);
				}
			}
			te.dwSize = sizeof(te);
		} while (Thread32Next(h, &te));
	}
	return vect;
}

DWORD GetThreadStartAddress(HANDLE processHandle, HANDLE hThread) {
	/* rewritten from https://github.com/cheat-engine/cheat-engine/blob/master/Cheat%20Engine/CEFuncProc.pas#L3080 */
	DWORD used = 0, ret = 0;
	DWORD stacktop = 0, result = 0;

	MODULEINFO mi;

	GetModuleInformation(processHandle, GetModuleHandle("kernel32.dll"), &mi, sizeof(mi));
	stacktop = (DWORD)GetThreadStackTopAddress_x86(processHandle, hThread);

	CloseHandle(hThread);

	if (stacktop)
	{
		DWORD* buf32 = new DWORD[4096];

		if (ReadProcessMemory(processHandle, (LPCVOID)(stacktop - 4096), buf32, 4096, NULL)) {
			for (int i = 4096 / 4 - 1; i >= 0; --i) {
				if (buf32[i] >= (DWORD)mi.lpBaseOfDll && buf32[i] <= (DWORD)mi.lpBaseOfDll + mi.SizeOfImage) {
					result = stacktop - 4096 + i * 4;
					break;
				}
			}
		}
		delete buf32;
	}
	return result;
}

DWORD	donne_la_threadstack()
{
	std::string pid = std::to_string(GetProcessId(TargetProcess));
	DWORD dwProcID;

	std::stringstream stringstream(pid);
	stringstream >> std::dec >> dwProcID;

	if (!dwProcID) {
		std::cerr << pid << " is not a valid process id (PID)" << std::endl;
		return EXIT_FAILURE;
	}

	HANDLE hProcHandle = NULL;

	//printf("PID %d (0x%x)\n", dwProcID, dwProcID);
	//std::cout << "Grabbing handle" << std::endl;
	hProcHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcID);

	if (hProcHandle == INVALID_HANDLE_VALUE || hProcHandle == NULL) {
		std::cerr << "Failed to open process -- invalid handle" << std::endl;
		std::cerr << "Error code: " << GetLastError() << std::endl;
		return EXIT_FAILURE;
	}
	else {
		std::cout << "Success" << std::endl;
	}

	std::vector<DWORD> threadId = threadList(dwProcID);
	int stackNum = 0;
	DWORD threadStartAddress;
	for (auto it = threadId.begin(); it != threadId.end(); ++it) {
		HANDLE threadHandle = OpenThread(THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, FALSE, *it);
		threadStartAddress = GetThreadStartAddress(hProcHandle, threadHandle);
		//printf("TID: 0x%04x = THREADSTACK%2d BASE ADDRESS: 0x%x\n", *it, stackNum, threadStartAddress);
		//if (stackNum == 0)
			break;
		//stackNum++;
	}

	return (threadStartAddress);
}

DWORD WINAPI	record_data(LPVOID dunno)
{
	int	cpt = -1;

	while (true)
	{
		if (cpt <= 0)
		{
			g_data.baseAddr = donne_la_threadstack();
			king.a_x	= memorySpace.getAddress(g_data.baseAddr - 0x980, { 0x9C, 0x32C, 0x10, 0x80, 0x8, 0x20, 0xB8 });
			king.a_y	= king.a_x + sizeof(float);
			king.a_vx	= king.a_y + sizeof(float);
			king.a_vy	= king.a_vx + sizeof(float);
			cpt = 1000;
			std::cout << "threadstack reloaded" << std::endl;
		}
		king.x = memorySpace.read<float>(king.a_x);
		king.y = memorySpace.read<float>(king.a_y);
		king.vx = memorySpace.read<float>(king.a_vx);
		king.vy = memorySpace.read<float>(king.a_vy);

		Sleep(100);
		--cpt;
	}
	return (0);
}

int				main()
{
	if (CreateConsole == false)
		ShowWindow(GetConsoleWindow(), SW_HIDE); 

	bool WindowFocus = false;
	while (WindowFocus == false) {
		DWORD ForegroundWindowProcessID;
		GetWindowThreadProcessId(GetForegroundWindow(), &ForegroundWindowProcessID);
		if (GetProcessId(TargetProcess) == ForegroundWindowProcessID) {
			Process::ID = GetCurrentProcessId();
			Process::Handle = GetCurrentProcess();
			Process::Hwnd = GetForegroundWindow();

			RECT TempRect;
			GetWindowRect(Process::Hwnd, &TempRect);
			Process::WindowWidth = TempRect.right - TempRect.left;
			Process::WindowHeight = TempRect.bottom - TempRect.top;
			Process::WindowLeft = TempRect.left;
			Process::WindowRight = TempRect.right;
			Process::WindowTop = TempRect.top;
			Process::WindowBottom = TempRect.bottom;

			char TempTitle[MAX_PATH];
			GetWindowText(Process::Hwnd, TempTitle, sizeof(TempTitle));
			Process::Title = TempTitle;

			char TempClassName[MAX_PATH];
			GetClassName(Process::Hwnd, TempClassName, sizeof(TempClassName));
			Process::ClassName = TempClassName;

			char TempPath[MAX_PATH];
			GetModuleFileNameEx(Process::Handle, NULL, TempPath, sizeof(TempPath));
			Process::Path = TempPath;

			WindowFocus = true;
		}
	}

	OverlayWindow::Name = RandomString(10).c_str();
	SetupWindow();
	DirectXInit();
	CreateThread(0, 0, ProcessCheck, 0, 0, 0);
	CreateThread(0, 0, record_data, 0, 0, 0);
	while (TRUE) {
		MainLoop();
	}
}