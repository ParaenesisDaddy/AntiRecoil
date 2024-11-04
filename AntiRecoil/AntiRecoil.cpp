#include <windows.h> 
#include <iostream>
#include <thread>
#include <random>
#include <cmath>
#include <iomanip> // For handling decimal precision
#include <chrono>  // For delay and animations
#include <fstream>
#include <string> // For handling file operations with strings

// Constants for timing and control
double RECOIL_STRENGTH = 2.0;  // How much to pull down the mouse
double RECOIL_DELAY = 10.0;    // Delay in milliseconds between movements
double RANDOMNESS = 1.0;       // Randomness to make mouse movement less predictable
double SMOOTHNESS = 1.0;       // Smoothing factor for more realistic movement
bool LEFT_MOVEMENT_ENABLED = false;  // Enable or disable left movement
bool RIGHT_MOVEMENT_ENABLED = false; // Enable or disable right movement
int MOUSE_BIND_PRIMARY = VK_LBUTTON;   // Default mouse bind is left mouse button
int MOUSE_BIND_SECONDARY = VK_XBUTTON1;  // Secondary bind for multi-button combinations

bool keepRunning = true;
std::default_random_engine generator;
std::uniform_real_distribution<double> distribution(-1.0, 1.0);
HWND hWnd;
bool isConsoleHidden = false;
bool recoilRunning = false;
std::string lastLoadedConfig = "config.txt"; // Default config file to auto-load

// Function to simulate mouse movement
void MoveMouse(int x, int y) {
    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    input.mi.dx = x;
    input.mi.dy = y;

    SendInput(1, &input, sizeof(INPUT));
}

// Function that continuously moves the mouse down while the mouse bind is held
void RecoilControl() {
    int direction = 1;
    while (keepRunning) {
        // Check if mouse bind is pressed
        bool primaryPressed = GetAsyncKeyState(MOUSE_BIND_PRIMARY) & 0x8000;
        bool secondaryPressed = (MOUSE_BIND_SECONDARY == 0) || (GetAsyncKeyState(MOUSE_BIND_SECONDARY) & 0x8000);

        if (primaryPressed && secondaryPressed) {
            double randomOffset = distribution(generator) * RANDOMNESS;
            int moveX = 0;
            int moveY = static_cast<int>((RECOIL_STRENGTH)+randomOffset);

            // Determine left/right movement if enabled
            if (LEFT_MOVEMENT_ENABLED && RIGHT_MOVEMENT_ENABLED) {
                moveX = static_cast<int>(direction + randomOffset);
                direction *= -1; // Alternate between left and right for a more human-like movement
            }
            else if (LEFT_MOVEMENT_ENABLED) {
                moveX = static_cast<int>(-1 + randomOffset);
            }
            else if (RIGHT_MOVEMENT_ENABLED) {
                moveX = static_cast<int>(1 + randomOffset);
            }

            MoveMouse(moveX, moveY);
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(RECOIL_DELAY + SMOOTHNESS)));
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

// Function to clear console
void ClearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Function to set the console output to support extended ASCII
void SetConsoleOutputCP_UTF8() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
}

// Function to hide the console window
void HideConsoleWindow() {
    if (hWnd != NULL) {
        ShowWindow(hWnd, SW_HIDE); // Hide the console window
        isConsoleHidden = true;
    }
}

// Function to show the console window
void ShowConsoleWindow() {
    if (hWnd != NULL) {
        ShowWindow(hWnd, SW_SHOW); // Show the console window
        isConsoleHidden = false;
    }
}

// Function to toggle the console window visibility
void ToggleConsoleWindow() {
    if (isConsoleHidden) {
        ShowConsoleWindow();
    }
    else {
        HideConsoleWindow();
    }
}

// Function to save current settings to a config file
void SaveConfig() {
    std::string filename;
    std::cout << "Enter the filename to save configuration: ";
    std::cin >> filename;
    std::ofstream configFile(filename);
    if (configFile.is_open()) {
        configFile << RECOIL_STRENGTH << "\n";
        configFile << RECOIL_DELAY << "\n";
        configFile << RANDOMNESS << "\n";
        configFile << SMOOTHNESS << "\n";
        configFile << (LEFT_MOVEMENT_ENABLED ? 1 : 0) << "\n";
        configFile << (RIGHT_MOVEMENT_ENABLED ? 1 : 0) << "\n";
        configFile << MOUSE_BIND_PRIMARY << "\n";
        configFile << MOUSE_BIND_SECONDARY << "\n";
        configFile.close();
        std::cout << "Configuration saved to " << filename << ".\n";
        lastLoadedConfig = filename;
    }
    else {
        std::cout << "Unable to open file for saving configuration.\n";
    }
}

// Function to load settings from a config file
void LoadConfig(const std::string& filename) {
    std::ifstream configFile(filename);
    if (configFile.is_open()) {
        configFile >> RECOIL_STRENGTH;
        configFile >> RECOIL_DELAY;
        configFile >> RANDOMNESS;
        configFile >> SMOOTHNESS;
        int leftMovement, rightMovement;
        configFile >> leftMovement;
        configFile >> rightMovement;
        LEFT_MOVEMENT_ENABLED = (leftMovement == 1);
        RIGHT_MOVEMENT_ENABLED = (rightMovement == 1);
        configFile >> MOUSE_BIND_PRIMARY;
        configFile >> MOUSE_BIND_SECONDARY;
        configFile.close();
        std::cout << "Configuration loaded from " << filename << ".\n";
        lastLoadedConfig = filename;
    }
    else {
        std::cout << "Unable to open file for loading configuration. Please check the filename or path and try again.\n";
    }
}

// Function to prompt user to load settings from a config file
void PromptLoadConfig() {
    std::string filename;
    std::cout << "Enter the filename of the configuration file to load (or type full path if not in the current directory): ";
    std::cin >> filename;
    LoadConfig(filename);
}

// Function to display the menu without animations
void DisplayMenu() {
    ClearConsole();
    SetConsoleOutputCP_UTF8();

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Set text color to cyan for ASCII art
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << R"(


$$\    $$\ $$\                               $$\ $$\   $$\                
$$ |   $$ |\__|                              $$ |\__|  $$ |              
$$ |   $$ |$$\  $$$$$$$\ $$\   $$\  $$$$$$\  $$ |$$\ $$$$$$\   $$\   $$\ 
\$$\  $$  |$$ |$$  _____|$$ |  $$ | \____$$\ $$ |$$ |\_$$  _|  $$ |  $$ |
 \$$\$$  / $$ |\$$$$$$\  $$ |  $$ | $$$$$$$ |$$ |$$ |  $$ |    $$ |  $$ |
  \$$$  /  $$ | \____$$\ $$ |  $$ |$$  __$$ |$$ |$$ |  $$ |$$\ $$ |  $$ |
   \$  /   $$ |$$$$$$$  |\$$$$$$  |\$$$$$$$ |$$ |$$ |  \$$$$  |\$$$$$$$ |
    \_/    \__|\_______/  \______/  \_______|\__|\__|   \____/  \____$$ |
                                                               $$\   $$ |
                                                               \$$$$$$  |
                                                                \______/
)";

    // Reset to default color for menu text
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << std::fixed << std::setprecision(1);  // Set precision to 1 decimal place

    // Set cyan color for the separators
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << "--------------------------------------------------\n";

    // Set white color for menu options
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << (recoilRunning ? "1. Stop Anti-Recoil\n" : "1. Start Anti-Recoil\n");
    std::cout << "2. Settings\n";
    std::cout << "3. Save Configuration\n";
    std::cout << "4. Load Configuration\n";
    std::cout << "5. Exit Program\n";

    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << "--------------------------------------------------\n\n";

    // Set white color for prompt
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << "Select an option: ";
}

// Function to display and handle settings without animations
void DisplaySettings() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int settingChoice;

    while (true) {
        ClearConsole();

        // Set cyan color for the header
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << "\n------ Settings Menu ------\n";

        // Set white color for options and grey for current values
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        std::cout << std::fixed << std::setprecision(1);  // Set precision to 1 decimal place

        std::cout << "1. Set Recoil Strength (Current: " << RECOIL_STRENGTH << ")\n";
        std::cout << "2. Set Recoil Delay (Current: " << RECOIL_DELAY << " ms)\n";
        std::cout << "3. Set Randomness (Current: " << RANDOMNESS << ")\n";
        std::cout << "4. Set Smoothness (Current: " << SMOOTHNESS << " ms)\n";
        std::cout << "5. Enable/Disable Left Movement (Current: " << (LEFT_MOVEMENT_ENABLED ? "Enabled" : "Disabled") << ")\n";
        std::cout << "6. Enable/Disable Right Movement (Current: " << (RIGHT_MOVEMENT_ENABLED ? "Enabled" : "Disabled") << ")\n";
        std::cout << "7. Set Mouse Bind (Current: " << (MOUSE_BIND_SECONDARY == 0 ? "Left Mouse Button" : (MOUSE_BIND_SECONDARY == VK_XBUTTON1 ? "Left Mouse Button + Side Button 1" : "Left Mouse Button + Side Button 2")) << ")\n";
        std::cout << "8. Back to Main Menu\n";

        // Set cyan color for the bottom separator
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << "---------------------------\n";

        // Set white color for prompt
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        std::cout << "Select an option: ";

        std::cin >> settingChoice;

        switch (settingChoice) {
        case 1:
            std::cout << "Enter new recoil strength: ";
            std::cin >> RECOIL_STRENGTH;
            std::cout << "Recoil strength set to " << RECOIL_STRENGTH << ".\n";
            break;
        case 2:
            std::cout << "Enter new recoil delay (in milliseconds): ";
            std::cin >> RECOIL_DELAY;
            std::cout << "Recoil delay set to " << RECOIL_DELAY << " ms.\n";
            break;
        case 3:
            std::cout << "Enter new randomness value: ";
            std::cin >> RANDOMNESS;
            std::cout << "Randomness set to " << RANDOMNESS << ".\n";
            break;
        case 4:
            std::cout << "Enter new smoothness value (in milliseconds): ";
            std::cin >> SMOOTHNESS;
            std::cout << "Smoothness set to " << SMOOTHNESS << " ms.\n";
            break;
        case 5:
            LEFT_MOVEMENT_ENABLED = !LEFT_MOVEMENT_ENABLED;
            std::cout << "Left movement " << (LEFT_MOVEMENT_ENABLED ? "enabled" : "disabled") << ".\n";
            break;
        case 6:
            RIGHT_MOVEMENT_ENABLED = !RIGHT_MOVEMENT_ENABLED;
            std::cout << "Right movement " << (RIGHT_MOVEMENT_ENABLED ? "enabled" : "disabled") << ".\n";
            break;
        case 7:
            int bindChoice;
            std::cout << "Select mouse bind:\n";
            std::cout << "1. Left Mouse Button\n";
            std::cout << "2. Left Mouse Button + Side Button 1\n";
            std::cout << "3. Left Mouse Button + Side Button 2\n";
            std::cout << "Select an option: ";
            std::cin >> bindChoice;
            switch (bindChoice) {
            case 1:
                MOUSE_BIND_PRIMARY = VK_LBUTTON;
                MOUSE_BIND_SECONDARY = 0;
                break;
            case 2:
                MOUSE_BIND_PRIMARY = VK_LBUTTON;
                MOUSE_BIND_SECONDARY = VK_XBUTTON1;
                break;
            case 3:
                MOUSE_BIND_PRIMARY = VK_LBUTTON;
                MOUSE_BIND_SECONDARY = VK_XBUTTON2;
                break;
            default:
                std::cout << "Invalid option. Defaulting to Left Mouse Button.\n";
                MOUSE_BIND_PRIMARY = VK_LBUTTON;
                MOUSE_BIND_SECONDARY = 0;
            }
            std::cout << "Mouse bind set.\n";
            break;
        case 8:
            return;
        default:
            std::cout << "Invalid option. Please try again.\n";
        }
    }
}

// Function to handle F7 key detection in a separate thread
void KeyDetection() {
    while (true) {
        if (GetAsyncKeyState(VK_F7) & 0x0001) {
            ToggleConsoleWindow();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Poll every 100 ms
    }
}

int main() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int choice;
    std::thread recoilThread;

    hWnd = GetConsoleWindow();

    std::thread keyDetectionThread(KeyDetection);
    keyDetectionThread.detach();

    // Auto-load the last configuration
    LoadConfig(lastLoadedConfig);

    while (true) {
        ClearConsole();
        DisplayMenu();
        std::cin >> choice;

        switch (choice) {
        case 1:
            if (!recoilRunning) {
                keepRunning = true;
                recoilThread = std::thread(RecoilControl);
                recoilRunning = true;
                SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                std::cout << "Anti-Recoil started.\n";
            }
            else {
                keepRunning = false;
                recoilThread.join();
                recoilRunning = false;
                SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                std::cout << "Anti-Recoil stopped.\n";
            }
            break;
        case 2:
            DisplaySettings();
            break;
        case 3:
            SaveConfig();
            break;
        case 4:
            PromptLoadConfig();
            break;
        case 5:
            if (recoilRunning) {
                keepRunning = false;
                recoilThread.join();
            }
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
            std::cout << "Exiting program.\n";
            return 0;
        default:
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
            std::cout << "Invalid option. Please try again.\n";
        }
    }
}
