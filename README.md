## Keyboard hook

This repository contains code for a keyboard interception application that intercepts low-level keyboard events and displays information about intercepted keys in a list. The code primarily focuses on the functionality of the keyboard hook rather than creating an aesthetically pleasing user interface.

### Functionality
The application provides the following features:

1. Keyboard event hooking: The code sets up a low-level keyboard hook using the `SetWindowsHookEx` function to hook keyboard events.
2. Key information display: When a key is pressed, the application captures key information such as virtual key code, scan code, and the corresponding character symbol. This information is then displayed in a list.
3. Writing to a file. The captured key information is also written to a file located in the C:\\KB_Hook\\KB_Hook\\CAPTURED.txt folder. Each keystroke is added as a new line in the file.
4. User interface. The application creates a main window with a list box and two buttons. The "LL Start" button activates the keyboard hook, and the "LL Stop" button deactivates it.

### Creation and launch
To build and run the application, follow these steps:

1. Run the compiled executable to run the application.
2. An application window will appear with a list and buttons.
3. Press the "LL Start" button to activate the keyboard hook and start capturing key events.
4. Press keys on the keyboard to view captured key information displayed in the list and written to a file.
5. Click the "LL Stop" button to disable the keyboard hook.

### Note
Make sure you update the file path in the code to match the desired location of the "CAPTURED.txt" file as all data from the capture is written to this document

Keep in mind that this code prioritizes keyboard hook functionality and does not have a polished user interface. The focus is on demonstrating the implementation of the hook itself rather than building a complex GUI.