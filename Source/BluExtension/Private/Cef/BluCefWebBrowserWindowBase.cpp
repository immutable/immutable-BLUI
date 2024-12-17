#include "Cef/BluCEFWebBrowserWindowBase.h"

#include "Widgets/SViewport.h"

THIRD_PARTY_INCLUDES_START
#include <include/cef_browser.h>
#include <include/cef_client.h>
THIRD_PARTY_INCLUDES_END

FWebBrowserDragRegion::FWebBrowserDragRegion(const FIntRect& InRect, bool bInDraggable):
	Rect(InRect),
	bDraggable(bInDraggable)
{
}

int32 FBluCefWebBrowserWindowBase::GetCefKeyboardModifiers(const FKeyEvent& KeyEvent)
{
	int32 Modifiers = GetCefInputModifiers(KeyEvent);

	const FKey Key = KeyEvent.GetKey();
	if (Key == EKeys::LeftAlt ||
		Key == EKeys::LeftCommand ||
		Key == EKeys::LeftControl ||
		Key == EKeys::LeftShift)
	{
		Modifiers |= EVENTFLAG_IS_LEFT;
	}
	if (Key == EKeys::RightAlt ||
		Key == EKeys::RightCommand ||
		Key == EKeys::RightControl ||
		Key == EKeys::RightShift)
	{
		Modifiers |= EVENTFLAG_IS_RIGHT;
	}
	if (Key == EKeys::NumPadZero ||
		Key == EKeys::NumPadOne ||
		Key == EKeys::NumPadTwo ||
		Key == EKeys::NumPadThree ||
		Key == EKeys::NumPadFour ||
		Key == EKeys::NumPadFive ||
		Key == EKeys::NumPadSix ||
		Key == EKeys::NumPadSeven ||
		Key == EKeys::NumPadEight ||
		Key == EKeys::NumPadNine)
	{
		Modifiers |= EVENTFLAG_IS_KEY_PAD;
	}

	return Modifiers;
}

int32 FBluCefWebBrowserWindowBase::GetCefMouseModifiers(const FPointerEvent& InMouseEvent)
{
	int32 Modifiers = GetCefInputModifiers(InMouseEvent);

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		Modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
	}
	if (InMouseEvent.IsMouseButtonDown(EKeys::MiddleMouseButton))
	{
		Modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
	}
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		Modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;
	}

	return Modifiers;
}

int32 FBluCefWebBrowserWindowBase::GetCefInputModifiers(const FInputEvent& InputEvent)
{
	int32 Modifiers = 0;

	if (InputEvent.IsShiftDown())
	{
		Modifiers |= EVENTFLAG_SHIFT_DOWN;
	}
	if (InputEvent.IsControlDown())
	{
#if PLATFORM_MAC
		// Slate swaps the flags for Command and Control on OSX, so we need to swap them back for CEF
		Modifiers |= EVENTFLAG_COMMAND_DOWN;
#else
		Modifiers |= EVENTFLAG_CONTROL_DOWN;
#endif
	}
	if (InputEvent.IsAltDown())
	{
		Modifiers |= EVENTFLAG_ALT_DOWN;
	}
	if (InputEvent.IsCommandDown())
	{
#if PLATFORM_MAC
		// Slate swaps the flags for Command and Control on OSX, so we need to swap them back for CEF
		Modifiers |= EVENTFLAG_CONTROL_DOWN;
#else
		Modifiers |= EVENTFLAG_COMMAND_DOWN;
#endif
	}
	if (InputEvent.AreCapsLocked())
	{
		Modifiers |= EVENTFLAG_CAPS_LOCK_ON;
	}

	return Modifiers;
}

bool FBluCefWebBrowserWindowBase::CanSupportAcceleratedPaint()
{
	static bool DisableAcceleratedPaint = FParse::Param(FCommandLine::Get(), TEXT("nocefaccelpaint"));
	if (DisableAcceleratedPaint)
	{
		return false;
	}

	static bool ForceAcceleratedPaint = FParse::Param(FCommandLine::Get(), TEXT("forcecefaccelpaint"));
	if (ForceAcceleratedPaint)
	{
		return true;
	}

	// Use off screen rendering so we can integrate with our windows
#if PLATFORM_LINUX
 	return false;
#elif PLATFORM_WINDOWS
#if PLATFORM_64BITS
	return false;
	/*
	 static bool Windows10OrAbove = FWindowsPlatformMisc::VerifyWindowsVersion(10, 0); //Win10
	 if (Windows10OrAbove == false)
	 {
		 return  -false;
	 }
 
	 // match the logic in GetStandardStandaloneRenderer() from StandaloneRenderer.cpp to check for the OGL slate renderer
	 if (FParse::Param(FCommandLine::Get(), TEXT("opengl")))
	 {
		 return false;
	 }
	 return true;*/

#else
 	return false; // 32-bit windows doesn't have the accelerated rendering patches applied, it can be done if needed
#endif
#elif PLATFORM_MAC
 	return false; // Needs RHI support for the CreateSharedHandleTexture call
#else
 	return false;
#endif
}

FBluCefWebBrowserWindowBase::FBluCefWebBrowserWindowBase(CefRefPtr<CefBrowser> NewCefBrowser) :
	InternalCefBrowser(NewCefBrowser),
	ViewportSize(FIntPoint::ZeroValue),
	ViewportDPIScaleFactor(1.0f),
	bIsDisabled(false),
	bIsHidden(false),
	bTickedLastFrame(true),
	bNeedsResize(false),
	bDraggingWindow(false),
	PreviousKeyDownEvent(),
	PreviousKeyUpEvent(),
	PreviousCharacterEvent(),
	bIgnoreKeyDownEvent(false),
	bIgnoreKeyUpEvent(false),
	bIgnoreCharacterEvent(false),
	bSupportsMouseWheel(true),
#if PLATFORM_WINDOWS || PLATFORM_MAC
	bInDirectHwndMode(false)
#endif
{
#if PLATFORM_WINDOWS || PLATFORM_MAC
	if (InternalCefBrowser->GetHost()->GetWindowHandle() != nullptr)
	{
		bInDirectHwndMode = true;
	}
#endif
}

FBluCefWebBrowserWindowBase::~FBluCefWebBrowserWindowBase()
{
}

bool FBluCefWebBrowserWindowBase::IsValid() const
{
	return InternalCefBrowser.get() != nullptr;
}

FIntPoint FBluCefWebBrowserWindowBase::GetViewportSize() const
{
	return FIntPoint::NoneValue;
}

FSlateShaderResource* FBluCefWebBrowserWindowBase::GetTexture(bool bIsPopup)
{
	return nullptr;
}

TSharedPtr<SWindow> FBluCefWebBrowserWindowBase::GetParentWindow() const
{
	TSharedPtr<SWindow> ParentWindowPtr = ParentWindow.Pin();
	return ParentWindowPtr;
}

bool FBluCefWebBrowserWindowBase::CanGoBack() const
{
	if (IsValid())
	{
		return InternalCefBrowser->CanGoBack();
	}
	return false;
}

bool FBluCefWebBrowserWindowBase::CanGoForward() const
{
	if (IsValid())
	{
		return InternalCefBrowser->CanGoForward();
	}
	return false;
}

FBluCefWebBrowserWindowBase::FOnToolTip& FBluCefWebBrowserWindowBase::OnToolTip()
{
	return ToolTipEvent;
}

IBluWebBrowserWindow::FOnUrlChanged& FBluCefWebBrowserWindowBase::OnUrlChanged()
{
	return UrlChangedEvent;
}

FBluCefWebBrowserWindowBase::FOnDragWindow& FBluCefWebBrowserWindowBase::OnDragWindow()
{
	return DragWindowDelegate;
}

void FBluCefWebBrowserWindowBase::SetViewportSize(FIntPoint WindowSize, FIntPoint WindowPos)
{
	// SetViewportSize is called from the browser viewport tick method, which means that since we are receiving ticks, we can mark the browser as visible.
	if (!bIsDisabled)
	{
		SetIsHidden(false);
	}
	bTickedLastFrame = true;

	float WindowDPIScaleFactor = 1.0f;
	if (TSharedPtr<SWindow> ParentWindowPtr = ParentWindow.Pin())
	{
		WindowDPIScaleFactor = ParentWindowPtr->GetNativeWindow()->GetDPIScaleFactor();
	}

	ViewportPos = WindowPos;

	// Ignore sizes that can't be seen as it forces CEF to re-render whole image
	if ((WindowSize.X > 0 && WindowSize.Y > 0 && ViewportSize != WindowSize) || WindowDPIScaleFactor != ViewportDPIScaleFactor)
	{
		bool bFirstSize = ViewportSize == FIntPoint::ZeroValue;
		ViewportSize = WindowSize;
		ViewportDPIScaleFactor = WindowDPIScaleFactor;

		if (IsValid())
		{
#if PLATFORM_WINDOWS
			HWND NativeHandle = InternalCefBrowser->GetHost()->GetWindowHandle();
			if (NativeHandle)
			{
				HWND Parent = ::GetParent(NativeHandle);
				// Position is in screen coordinates, so we'll need to get the parent window location first.
				RECT ParentRect = {0, 0, 0, 0};
				if (Parent)
				{
					::GetWindowRect(Parent, &ParentRect);
				}

				FIntPoint WindowSizeScaled = (FVector2D(WindowSize) * WindowDPIScaleFactor).IntPoint();

				::SetWindowPos(NativeHandle, 0, WindowPos.X - ParentRect.left, WindowPos.Y - ParentRect.top, WindowSizeScaled.X, WindowSizeScaled.Y, 0);
			}
#elif PLATFORM_MAC
			CefWindowHandle NativeWindowHandle = InternalCefBrowser->GetHost()->GetWindowHandle();
			if (NativeWindowHandle)
			{
				NSView* browserView = CAST_CEF_WINDOW_HANDLE_TO_NSVIEW(NativeWindowHandle);
				if (TSharedPtr<SWindow> ParentWindowPtr = ParentWindow.Pin())
				{
					NSWindow* parentWindow = (NSWindow*)ParentWindowPtr->GetNativeWindow()->GetOSWindowHandle();

					const FVector2D CocoaPosition = FMacApplication::ConvertSlatePositionToCocoa(WindowPos.X, WindowPos.Y);
					NSRect parentFrame = [parentWindow frame];
					NSRect Rect = NSMakeRect(CocoaPosition.X - parentFrame.origin.x, (CocoaPosition.Y - parentFrame.origin.y) - WindowSize.Y, FMath::Max(WindowSize.X, 1), FMath::Max(WindowSize.Y, 1));
					Rect = [parentWindow frameRectForContentRect : Rect];

					[browserView setFrame : Rect] ;
				}
			}
#endif

			if (bFirstSize)
			{
				InternalCefBrowser->GetHost()->WasResized();
			}
			else
			{
				bNeedsResize = true;
			}
		}
	}
}

void FBluCefWebBrowserWindowBase::SetParentWindow(TSharedPtr<SWindow> Window)
{
	ParentWindow = Window;
#if PLATFORM_WINDOWS
	if (IsValid())
	{
		CefRefPtr<CefBrowserHost> BrowserHost = InternalCefBrowser->GetHost();

		HWND NativeWindowHandle = BrowserHost->GetWindowHandle();
		if (NativeWindowHandle != nullptr)
		{
			TSharedPtr<SWindow> ParentWindowPtr = ParentWindow.Pin();
			void* ParentWindowHandle = (ParentWindow.IsValid() && ParentWindowPtr->GetNativeWindow().IsValid()) ? ParentWindowPtr->GetNativeWindow()->GetOSWindowHandle() : nullptr;
			if (ParentWindowHandle != nullptr)
			{
				// When rendering directly to a HWND update its parent windown
				::SetParent(NativeWindowHandle, (HWND)ParentWindowHandle);
			}
		}
	}
#endif
}

TSharedPtr<SViewport> FBluCefWebBrowserWindowBase::CreateWidget()
{
	TSharedRef<SViewport> BrowserWidgetRef =
		SNew(SViewport)
		.EnableGammaCorrection(false);

	return BrowserWidgetRef;
}

FReply FBluCefWebBrowserWindowBase::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& CharacterEvent)
{
	if (IsValid() && !BlockInputInDirectHwndMode() && !bIgnoreCharacterEvent)
	{
		PreviousCharacterEvent = CharacterEvent;
		CefKeyEvent KeyEvent;
#if PLATFORM_MAC || PLATFORM_LINUX
 		KeyEvent.character = InCharacterEvent.GetCharacter();
 		KeyEvent.windows_key_code = InCharacterEvent.GetCharacter();
#else
		KeyEvent.windows_key_code = CharacterEvent.GetCharacter();
#endif
		KeyEvent.type = KEYEVENT_CHAR;
		KeyEvent.modifiers = GetCefInputModifiers(CharacterEvent);
#if PLATFORM_WINDOWS
		if (CharacterEvent.IsAltDown() && CharacterEvent.IsControlDown())
		{
			// For german and other keyboards with an AltGR state, windows sets alt and left control down 
			// See OsrWindowWin::OnKeyEvent in 
			//https://bitbucket.org/chromiumembedded/cef/raw/c4baba880e0b28ce82845275b328a12b2407e2f0/tests/cefclient/browser/osr_window_win.cc 
			// from which the concept behind this check was taken
			HKL CurrentKBLayout = ::GetKeyboardLayout(0);
			SHORT ScanResult = ::VkKeyScanExW(CharacterEvent.GetCharacter(), CurrentKBLayout);
			if (((ScanResult >> 8) & 0xFF) == (2 | 4))
			{
				// ctrl-alt pressed from this single character event so convert to AltGR	
				KeyEvent.modifiers &= ~(EVENTFLAG_CONTROL_DOWN | EVENTFLAG_ALT_DOWN);
				KeyEvent.modifiers |= EVENTFLAG_ALTGR_DOWN;
			}
		}
#endif
		InternalCefBrowser->GetHost()->SendKeyEvent(KeyEvent);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply FBluCefWebBrowserWindowBase::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
	if (IsValid() && !BlockInputInDirectHwndMode() && !bIgnoreKeyDownEvent)
	{
#if PLATFORM_MAC
 		if(FilterSystemKeyChord(InKeyEvent))
 			return false;
#endif
		PreviousKeyDownEvent = KeyEvent;
		CefKeyEvent CefKeyEvent;
		PopulateCefKeyEvent(KeyEvent, CefKeyEvent);
		CefKeyEvent.type = KEYEVENT_RAWKEYDOWN;
		InternalCefBrowser->GetHost()->SendKeyEvent(CefKeyEvent);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply FBluCefWebBrowserWindowBase::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
	if (IsValid() && !BlockInputInDirectHwndMode() && !bIgnoreKeyUpEvent)
	{
#if PLATFORM_MAC
 		if(FilterSystemKeyChord(InKeyEvent))
 			return false;
#endif
		PreviousKeyUpEvent = KeyEvent;
		CefKeyEvent CefKeyEvent;
		PopulateCefKeyEvent(KeyEvent, CefKeyEvent);
		CefKeyEvent.type = KEYEVENT_KEYUP;
		InternalCefBrowser->GetHost()->SendKeyEvent(CefKeyEvent);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply FBluCefWebBrowserWindowBase::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup)
{
	FReply Reply = FReply::Unhandled();
	if (IsValid() && !BlockInputInDirectHwndMode())
	{
		FKey Button = MouseEvent.GetEffectingButton();
		// CEF only supports left, right, and middle mouse buttons
		bool bIsCefSupportedButton = (Button == EKeys::LeftMouseButton || Button == EKeys::RightMouseButton || Button == EKeys::MiddleMouseButton);

		if (bIsCefSupportedButton)
		{
			CefBrowserHost::MouseButtonType Type = (Button == EKeys::LeftMouseButton ? MBT_LEFT : (Button == EKeys::RightMouseButton ? MBT_RIGHT : MBT_MIDDLE));

			CefMouseEvent Event = GetCefMouseEvent(MyGeometry, MouseEvent, bIsPopup);

			// If the click happened inside a drag region we enable window dragging which will start firing OnDragWindow events on mouse move
			if (Type == MBT_LEFT && IsInDragRegion(FIntPoint(Event.x, Event.y)))
			{
				bDraggingWindow = true;
			}

			InternalCefBrowser->GetHost()->SendMouseClickEvent(Event, Type, false, 1);
			Reply = FReply::Handled();
		}
	}
	return Reply;
}

FReply FBluCefWebBrowserWindowBase::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup)
{
	FReply Reply = FReply::Unhandled();
	if (IsValid() && !BlockInputInDirectHwndMode())
	{
		FKey Button = MouseEvent.GetEffectingButton();
		// CEF only supports left, right, and middle mouse buttons
		bool bIsCefSupportedButton = (Button == EKeys::LeftMouseButton || Button == EKeys::RightMouseButton || Button == EKeys::MiddleMouseButton);

		if (bIsCefSupportedButton)
		{
			CefBrowserHost::MouseButtonType Type = (Button == EKeys::LeftMouseButton ? MBT_LEFT : (Button == EKeys::RightMouseButton ? MBT_RIGHT : MBT_MIDDLE));

			if (Type == MBT_LEFT)
			{
				bDraggingWindow = false;
			}

			CefMouseEvent Event = GetCefMouseEvent(MyGeometry, MouseEvent, bIsPopup);
			InternalCefBrowser->GetHost()->SendMouseClickEvent(Event, Type, true, 1);
			Reply = FReply::Handled();
		}
		else if (Button == EKeys::ThumbMouseButton && bThumbMouseButtonNavigation)
		{
			if (CanGoBack())
			{
				GoBack();
				Reply = FReply::Handled();
			}
		}
		else if (Button == EKeys::ThumbMouseButton2 && bThumbMouseButtonNavigation)
		{
			if (CanGoForward())
			{
				GoForward();
				Reply = FReply::Handled();
			}
		}
	}
	return Reply;
}

FReply FBluCefWebBrowserWindowBase::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup)
{
	FReply Reply = FReply::Unhandled();
	if (IsValid() && !BlockInputInDirectHwndMode())
	{
		CefMouseEvent Event = GetCefMouseEvent(MyGeometry, MouseEvent, bIsPopup);

		bool bEventConsumedByDragCallback = false;
		if (bDraggingWindow && OnDragWindow().IsBound())
		{
			if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
			{
				bEventConsumedByDragCallback = OnDragWindow().Execute(MouseEvent);
			}
			else
			{
				bDraggingWindow = false;
			}
		}

		if (!bEventConsumedByDragCallback)
		{
			InternalCefBrowser->GetHost()->SendMouseMoveEvent(Event, false);
		}

		Reply = FReply::Handled();
	}
	return Reply;
}

void FBluCefWebBrowserWindowBase::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
}

void FBluCefWebBrowserWindowBase::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	// Ensure we clear any tooltips if the mouse leaves the window.
	SetToolTip(CefString());
	// We have no geometry here to convert our mouse event to local space so we just make a dummy event and set the moueLeave param to true
	CefMouseEvent DummyEvent;
	if (IsValid() && !BlockInputInDirectHwndMode())
	{
		InternalCefBrowser->GetHost()->SendMouseMoveEvent(DummyEvent, true);
	}
}

FReply FBluCefWebBrowserWindowBase::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup)
{
	FReply Reply = FReply::Unhandled();
	if (IsValid() && bSupportsMouseWheel && !BlockInputInDirectHwndMode())
	{
#if PLATFORM_WINDOWS
		// The original delta is reduced so this should bring it back to what CEF expects
		// see WindowsApplication.cpp , case WM_MOUSEWHEEL:
		const float SpinFactor = 120.0f;
#else
 		// other OS's seem to want us to scale by "line height" here, so pick a magic number
 		// 50 matches a single mouse wheel tick in movement as compared to Chrome
 		const float SpinFactor = 50.0f;
#endif
		const float TrueDelta = MouseEvent.GetWheelDelta() * SpinFactor;
		if (fabs(TrueDelta) > 0.001f)
		{
			CefMouseEvent Event = GetCefMouseEvent(MyGeometry, MouseEvent, bIsPopup);
			InternalCefBrowser->GetHost()->SendMouseWheelEvent(Event, MouseEvent.IsShiftDown() ? TrueDelta : 0, !MouseEvent.IsShiftDown() ? TrueDelta : 0);
		}
		Reply = FReply::Handled();
	}
	return Reply;
}

FReply FBluCefWebBrowserWindowBase::OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup)
{
	FReply Reply = FReply::Unhandled();
	if (IsValid() && !BlockInputInDirectHwndMode())
	{
		FKey Button = MouseEvent.GetEffectingButton();
		// CEF only supports left, right, and middle mouse buttons
		bool bIsCefSupportedButton = (Button == EKeys::LeftMouseButton || Button == EKeys::RightMouseButton || Button == EKeys::MiddleMouseButton);

		if (bIsCefSupportedButton)
		{
			CefBrowserHost::MouseButtonType Type = (Button == EKeys::LeftMouseButton ? MBT_LEFT : (Button == EKeys::RightMouseButton ? MBT_RIGHT : MBT_MIDDLE));

			CefMouseEvent Event = GetCefMouseEvent(MyGeometry, MouseEvent, bIsPopup);
			InternalCefBrowser->GetHost()->SendMouseClickEvent(Event, Type, false, 2);
			Reply = FReply::Handled();
		}
	}
	return Reply;
}

FReply FBluCefWebBrowserWindowBase::OnTouchGesture(const FGeometry& MyGeometry, const FPointerEvent& GestureEvent, bool bIsPopup)
{
	FReply Reply = FReply::Unhandled();
	if (IsValid() && bSupportsMouseWheel && !BlockInputInDirectHwndMode())
	{
		const EGestureEvent GestureType = GestureEvent.GetGestureType();
		const FVector2D& GestureDelta = GestureEvent.GetGestureDelta();
		if (GestureType == EGestureEvent::Scroll)
		{
			CefMouseEvent Event = GetCefMouseEvent(MyGeometry, GestureEvent, bIsPopup);
			InternalCefBrowser->GetHost()->SendMouseWheelEvent(Event, GestureDelta.X, GestureDelta.Y);
			Reply = FReply::Handled();
		}
	}

	return Reply;
}

void FBluCefWebBrowserWindowBase::GoBack()
{
	if (IsValid())
	{
		InternalCefBrowser->GoBack();
	}
}

void FBluCefWebBrowserWindowBase::GoForward()
{
	if (IsValid())
	{
		InternalCefBrowser->GoForward();
	}
}

CefMouseEvent FBluCefWebBrowserWindowBase::GetCefMouseEvent(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup)
{
	CefMouseEvent Event;

	FGeometry MouseGeometry = MyGeometry;

	float DPIScale = MouseGeometry.Scale;
	if (TSharedPtr<SWindow> ParentWindowPtr = ParentWindow.Pin())
	{
		DPIScale /= ParentWindowPtr->GetNativeWindow()->GetDPIScaleFactor();
	}

	FVector2D LocalPos = MouseGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()) * DPIScale;
	if (bIsPopup)
	{
		LocalPos += PopupPosition;
	}
	Event.x = LocalPos.X;
	Event.y = LocalPos.Y;
	Event.modifiers = GetCefMouseModifiers(MouseEvent);

	return Event;
}

bool FBluCefWebBrowserWindowBase::IsInDragRegion(const FIntPoint& Point)
{
	// Here we traverse the array of drag regions backwards because we assume the drag regions are z ordered such that 
	// the end of the list contains the drag regions of the top most elements of the web page.  We can stop checking
	// once we hit a region that contains our point.
	for (int32 Idx = DragRegions.Num() - 1; Idx >= 0; --Idx)
	{
		if (DragRegions[Idx].Rect.Contains(Point))
		{
			return DragRegions[Idx].bDraggable;
		}
	}
	return false;
}

void FBluCefWebBrowserWindowBase::PopulateCefKeyEvent(const FKeyEvent& InKeyEvent, CefKeyEvent& OutKeyEvent)
{
#if PLATFORM_MAC
	OutKeyEvent.native_key_code = InKeyEvent.GetKeyCode();

	FKey Key = InKeyEvent.GetKey();
	if (Key == EKeys::BackSpace)
	{
		OutKeyEvent.unmodified_character = kBackspaceCharCode;
	}
	else if (Key == EKeys::Tab)
	{
		OutKeyEvent.unmodified_character = kTabCharCode;
	}
	else if (Key == EKeys::Enter)
	{
		OutKeyEvent.unmodified_character = kReturnCharCode;
	}
	else if (Key == EKeys::Pause)
	{
		OutKeyEvent.unmodified_character = NSPauseFunctionKey;
	}
	else if (Key == EKeys::Escape)
	{
		OutKeyEvent.unmodified_character = kEscapeCharCode;
	}
	else if (Key == EKeys::PageUp)
	{
		OutKeyEvent.unmodified_character = NSPageUpFunctionKey;
	}
	else if (Key == EKeys::PageDown)
	{
		OutKeyEvent.unmodified_character = NSPageDownFunctionKey;
	}
	else if (Key == EKeys::End)
	{
		OutKeyEvent.unmodified_character = NSEndFunctionKey;
	}
	else if (Key == EKeys::Home)
	{
		OutKeyEvent.unmodified_character = NSHomeFunctionKey;
	}
	else if (Key == EKeys::Left)
	{
		OutKeyEvent.unmodified_character = NSLeftArrowFunctionKey;
	}
	else if (Key == EKeys::Up)
	{
		OutKeyEvent.unmodified_character = NSUpArrowFunctionKey;
	}
	else if (Key == EKeys::Right)
	{
		OutKeyEvent.unmodified_character = NSRightArrowFunctionKey;
	}
	else if (Key == EKeys::Down)
	{
		OutKeyEvent.unmodified_character = NSDownArrowFunctionKey;
	}
	else if (Key == EKeys::Insert)
	{
		OutKeyEvent.unmodified_character = NSInsertFunctionKey;
	}
	else if (Key == EKeys::Delete)
	{
		OutKeyEvent.unmodified_character = kDeleteCharCode;
	}
	else if (Key == EKeys::F1)
	{
		OutKeyEvent.unmodified_character = NSF1FunctionKey;
	}
	else if (Key == EKeys::F2)
	{
		OutKeyEvent.unmodified_character = NSF2FunctionKey;
	}
	else if (Key == EKeys::F3)
	{
		OutKeyEvent.unmodified_character = NSF3FunctionKey;
	}
	else if (Key == EKeys::F4)
	{
		OutKeyEvent.unmodified_character = NSF4FunctionKey;
	}
	else if (Key == EKeys::F5)
	{
		OutKeyEvent.unmodified_character = NSF5FunctionKey;
	}
	else if (Key == EKeys::F6)
	{
		OutKeyEvent.unmodified_character = NSF6FunctionKey;
	}
	else if (Key == EKeys::F7)
	{
		OutKeyEvent.unmodified_character = NSF7FunctionKey;
	}
	else if (Key == EKeys::F8)
	{
		OutKeyEvent.unmodified_character = NSF8FunctionKey;
	}
	else if (Key == EKeys::F9)
	{
		OutKeyEvent.unmodified_character = NSF9FunctionKey;
	}
	else if (Key == EKeys::F10)
	{
		OutKeyEvent.unmodified_character = NSF10FunctionKey;
	}
	else if (Key == EKeys::F11)
	{
		OutKeyEvent.unmodified_character = NSF11FunctionKey;
	}
	else if (Key == EKeys::F12)
	{
		OutKeyEvent.unmodified_character = NSF12FunctionKey;
	}
	else if (Key == EKeys::CapsLock)
	{
		OutKeyEvent.unmodified_character = 0;
		OutKeyEvent.native_key_code = kVK_CapsLock;
	}
	else if (Key.IsModifierKey())
	{
		// Setting both unmodified_character and character to 0 tells CEF that it needs to generate a NSFlagsChanged event instead of NSKeyDown/Up
		OutKeyEvent.unmodified_character = 0;

		// CEF expects modifier key codes as one of the Carbon kVK_* key codes.
		if (Key == EKeys::LeftCommand)
		{
			OutKeyEvent.native_key_code = kVK_Command;
		}
		else if (Key == EKeys::LeftShift)
		{
			OutKeyEvent.native_key_code = kVK_Shift;
		}
		else if (Key == EKeys::LeftAlt)
		{
			OutKeyEvent.native_key_code = kVK_Option;
		}
		else if (Key == EKeys::LeftControl)
		{
			OutKeyEvent.native_key_code = kVK_Control;
		}
		else if (Key == EKeys::RightCommand)
		{
			// There isn't a separate code for the right hand command key defined, but CEF seems to use the unused value before the left command keycode
			OutKeyEvent.native_key_code = kVK_Command-1;
		}
		else if (Key == EKeys::RightShift)
		{
			OutKeyEvent.native_key_code = kVK_RightShift;
		}
		else if (Key == EKeys::RightAlt)
		{
			OutKeyEvent.native_key_code = kVK_RightOption;
		}
		else if (Key == EKeys::RightControl)
		{
			OutKeyEvent.native_key_code = kVK_RightControl;
		}
	}
	else
	{
		OutKeyEvent.unmodified_character = InKeyEvent.GetCharacter();
	}
	OutKeyEvent.character = OutKeyEvent.unmodified_character;

#elif PLATFORM_LINUX
	OutKeyEvent.native_key_code = InKeyEvent.GetKeyCode();
	FKey Key = InKeyEvent.GetKey();
	// helper macro so we can fill in all the A-Z, 0-9 keys
#define LETTER_KEY_MACRO(val, vkey) else if(Key == EKeys::val) \
	{ \
		OutKeyEvent.unmodified_character = InKeyEvent.GetCharacter(); \
		OutKeyEvent.windows_key_code = vkey; \
	} \

	if (Key == EKeys::BackSpace)
	{
		OutKeyEvent.windows_key_code = VKEY_BACK;
	}
	else if (Key == EKeys::Tab)
	{
		OutKeyEvent.windows_key_code = VKEY_TAB;
	}
	else if (Key == EKeys::Enter)
	{
		OutKeyEvent.windows_key_code = VKEY_RETURN;
	}
	else if (Key == EKeys::Pause)
	{
		OutKeyEvent.windows_key_code = VKEY_PAUSE;
	}
	else if (Key == EKeys::Escape)
	{
		OutKeyEvent.windows_key_code = VKEY_ESCAPE;
	}
	else if (Key == EKeys::PageUp)
	{
		OutKeyEvent.windows_key_code = VKEY_PRIOR;
	}
	else if (Key == EKeys::PageDown)
	{
		OutKeyEvent.windows_key_code = VKEY_NEXT;
	}
	else if (Key == EKeys::End)
	{
		OutKeyEvent.windows_key_code = VKEY_END;
	}
	else if (Key == EKeys::Home)
	{
		OutKeyEvent.windows_key_code = VKEY_HOME;
	}
	else if (Key == EKeys::Left)
	{
		OutKeyEvent.windows_key_code = VKEY_LEFT;
	}
	else if (Key == EKeys::Up)
	{
		OutKeyEvent.windows_key_code = VKEY_UP;
	}
	else if (Key == EKeys::Right)
	{
		OutKeyEvent.windows_key_code = VKEY_RIGHT;
	}
	else if (Key == EKeys::Down)
	{
		OutKeyEvent.windows_key_code = VKEY_DOWN;
	}
	else if (Key == EKeys::Insert)
	{
		OutKeyEvent.windows_key_code = VKEY_INSERT;
	}
	else if (Key == EKeys::Delete)
	{
		OutKeyEvent.windows_key_code = VKEY_DELETE;
	}
	else if (Key == EKeys::F1)
	{
		OutKeyEvent.windows_key_code = VKEY_F1;
	}
	else if (Key == EKeys::F2)
	{
		OutKeyEvent.windows_key_code = VKEY_F2;
	}
	else if (Key == EKeys::F3)
	{
		OutKeyEvent.windows_key_code = VKEY_F3;
	}
	else if (Key == EKeys::F4)
	{
		OutKeyEvent.windows_key_code = VKEY_F4;
	}
	else if (Key == EKeys::F5)
	{
		OutKeyEvent.windows_key_code = VKEY_F5;
	}
	else if (Key == EKeys::F6)
	{
		OutKeyEvent.windows_key_code = VKEY_F6;
	}
	else if (Key == EKeys::F7)
	{
		OutKeyEvent.windows_key_code = VKEY_F7;
	}
	else if (Key == EKeys::F8)
	{
		OutKeyEvent.windows_key_code = VKEY_F8;
	}
	else if (Key == EKeys::F9)
	{
		OutKeyEvent.windows_key_code = VKEY_F9;
	}
	else if (Key == EKeys::F10)
	{
		OutKeyEvent.windows_key_code = VKEY_F10;
	}
	else if (Key == EKeys::F11)
	{
		OutKeyEvent.windows_key_code = VKEY_F11;
	}
	else if (Key == EKeys::F12)
	{
		OutKeyEvent.windows_key_code = VKEY_F12;
	}
	else if (Key == EKeys::CapsLock)
	{
		OutKeyEvent.windows_key_code = VKEY_CAPITAL;
	}
	else if (Key == EKeys::LeftCommand)
	{
		OutKeyEvent.windows_key_code = VKEY_MENU;
	}
	else if (Key == EKeys::LeftShift)
	{
		OutKeyEvent.windows_key_code = VKEY_SHIFT;
	}
	else if (Key == EKeys::LeftAlt)
	{
		OutKeyEvent.windows_key_code = VKEY_MENU;
	}
	else if (Key == EKeys::LeftControl)
	{
		OutKeyEvent.windows_key_code = VKEY_CONTROL;
	}
	else if (Key == EKeys::RightCommand)
	{
		OutKeyEvent.windows_key_code = VKEY_MENU;
	}
	else if (Key == EKeys::RightShift)
	{
		OutKeyEvent.windows_key_code = VKEY_SHIFT;
	}
	else if (Key == EKeys::RightAlt)
	{
		OutKeyEvent.windows_key_code = VKEY_MENU;
	}
	else if (Key == EKeys::RightControl)
	{
		OutKeyEvent.windows_key_code = VKEY_CONTROL;
	}
	else if(Key == EKeys::NumPadOne)
	{
		OutKeyEvent.windows_key_code = VKEY_NUMPAD1;
	}
	else if(Key == EKeys::NumPadTwo)
	{
		OutKeyEvent.windows_key_code = VKEY_NUMPAD2;
	}
	else if(Key == EKeys::NumPadThree)
	{
		OutKeyEvent.windows_key_code = VKEY_NUMPAD3;
	}
	else if(Key == EKeys::NumPadFour)
	{
		OutKeyEvent.windows_key_code = VKEY_NUMPAD4;
	}
	else if(Key == EKeys::NumPadFive)
	{
		OutKeyEvent.windows_key_code = VKEY_NUMPAD5;
	}
	else if(Key == EKeys::NumPadSix)
	{
		OutKeyEvent.windows_key_code = VKEY_NUMPAD6;
	}
	else if(Key == EKeys::NumPadSeven)
	{
		OutKeyEvent.windows_key_code = VKEY_NUMPAD7;
	}
	else if(Key == EKeys::NumPadEight)
	{
		OutKeyEvent.windows_key_code = VKEY_NUMPAD8;
	}
	else if(Key == EKeys::NumPadNine)
	{
		OutKeyEvent.windows_key_code = VKEY_NUMPAD9;
	}
	else if(Key == EKeys::NumPadZero)
	{
		OutKeyEvent.windows_key_code = VKEY_NUMPAD0;
	}
	LETTER_KEY_MACRO( A, VKEY_A)
	LETTER_KEY_MACRO( B, VKEY_B)
	LETTER_KEY_MACRO( C, VKEY_C)
	LETTER_KEY_MACRO( D, VKEY_D)
	LETTER_KEY_MACRO( E, VKEY_E)
	LETTER_KEY_MACRO( F, VKEY_F)
	LETTER_KEY_MACRO( G, VKEY_G)
	LETTER_KEY_MACRO( H, VKEY_H)
	LETTER_KEY_MACRO( I, VKEY_I)
	LETTER_KEY_MACRO( J, VKEY_J)
	LETTER_KEY_MACRO( K, VKEY_K)
	LETTER_KEY_MACRO( L, VKEY_L)
	LETTER_KEY_MACRO( M, VKEY_M)
	LETTER_KEY_MACRO( N, VKEY_N)
	LETTER_KEY_MACRO( O, VKEY_O)
	LETTER_KEY_MACRO( P, VKEY_P)
	LETTER_KEY_MACRO( Q, VKEY_Q)
	LETTER_KEY_MACRO( R, VKEY_R)
	LETTER_KEY_MACRO( S, VKEY_S)
	LETTER_KEY_MACRO( T, VKEY_T)
	LETTER_KEY_MACRO( U, VKEY_U)
	LETTER_KEY_MACRO( V, VKEY_V)
	LETTER_KEY_MACRO( W, VKEY_W)
	LETTER_KEY_MACRO( X, VKEY_X)
	LETTER_KEY_MACRO( Y, VKEY_Y)
	LETTER_KEY_MACRO( Z, VKEY_Z)
	LETTER_KEY_MACRO( Zero, VKEY_0)
	LETTER_KEY_MACRO( One, VKEY_1)
	LETTER_KEY_MACRO( Two, VKEY_2)
	LETTER_KEY_MACRO( Three, VKEY_3)
	LETTER_KEY_MACRO( Four, VKEY_4)
	LETTER_KEY_MACRO( Five, VKEY_5)
	LETTER_KEY_MACRO( Six, VKEY_6)
	LETTER_KEY_MACRO( Seven, VKEY_7)
	LETTER_KEY_MACRO( Eight, VKEY_8)
	LETTER_KEY_MACRO( Nine, VKEY_9)
	else
	{
		OutKeyEvent.unmodified_character = InKeyEvent.GetCharacter();
		OutKeyEvent.windows_key_code = VKEY_UNKNOWN;
	}
#else
	OutKeyEvent.windows_key_code = InKeyEvent.GetKeyCode();
#endif

	OutKeyEvent.modifiers = GetCefKeyboardModifiers(InKeyEvent);
	//UE_LOG(LogWebBrowser, Log, TEXT("Modifiers: %i %i %i") , OutKeyEvent.unmodified_character, OutKeyEvent.windows_key_code, OutKeyEvent.modifiers);
}

bool FBluCefWebBrowserWindowBase::BlockInputInDirectHwndMode() const
{
#if PLATFORM_WINDOWS
	return bInDirectHwndMode;
#elif PLATFORM_MAC
 	return bInDirectHwndMode;
#endif

	return false;
}

void FBluCefWebBrowserWindowBase::SetToolTip(const CefString& InToolTip)
{
	FString NewToolTipText = WCHAR_TO_TCHAR(InToolTip.ToWString().c_str());
	if (ToolTipText != NewToolTipText)
	{
		ToolTipText = NewToolTipText;
		OnToolTip().Broadcast(ToolTipText);
	}
}

void FBluCefWebBrowserWindowBase::SetIsHidden(bool bValue)
{
	if (bIsHidden == bValue)
	{
		return;
	}
	bIsHidden = bValue;
	if (IsValid())
	{
		CefRefPtr<CefBrowserHost> BrowserHost = InternalCefBrowser->GetHost();
#if PLATFORM_WINDOWS
		HWND NativeWindowHandle = BrowserHost->GetWindowHandle();
		if (NativeWindowHandle != nullptr)
		{
			// When rendering directly into a subwindow, we must hide the native window when fully obscured
			::ShowWindow(NativeWindowHandle, bIsHidden ? SW_HIDE : SW_SHOW);

			if (bIsHidden)
			{
				TSharedPtr<SWindow> ParentWindowPtr = ParentWindow.Pin();
				if (::IsWindowEnabled(NativeWindowHandle) && ParentWindowPtr.IsValid())
				{
					::SetFocus((HWND)ParentWindowPtr->GetNativeWindow()->GetOSWindowHandle());
				}
				// when hidden also resize the window to 0x0 to further reduce resource usage. This copies the
				// behavior of the CefClient code, see browser_window_std_win.cc in the ::Hide() function. This code
				// is also required for the HTML5 visibility API to work 
				SetWindowPos(NativeWindowHandle, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
			}
			else
			{
				// restore the window to its right size/position
				HWND Parent = ::GetParent(NativeWindowHandle);
				// Position is in screen coordinates, so we'll need to get the parent window location first.
				RECT ParentRect = {0, 0, 0, 0};
				if (Parent)
				{
					::GetWindowRect(Parent, &ParentRect);
				}

				FIntPoint WindowSizeScaled = (FVector2D(ViewportSize) * ViewportDPIScaleFactor).IntPoint();

				::SetWindowPos(NativeWindowHandle, 0, ViewportPos.X - ParentRect.left, ViewportPos.Y - ParentRect.top, WindowSizeScaled.X, WindowSizeScaled.Y, 0);
			}
		}
		else
		{
#elif PLATFORM_MAC
		CefWindowHandle NativeWindowHandle = BrowserHost->GetWindowHandle();
		if (NativeWindowHandle != nullptr)
		{
			NSView *browserView = CAST_CEF_WINDOW_HANDLE_TO_NSVIEW(NativeWindowHandle);
			if(bIsHidden)
			{
				[browserView setHidden:YES];
			}
			else
			{
				[browserView setHidden:NO];
			}
			
		}
		else
		{
#endif
			BrowserHost->WasHidden(bIsHidden);
#if PLATFORM_WINDOWS || PLATFORM_MAC
		}
#endif
	}
}