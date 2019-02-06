#pragma once

#define	DAYLIGHT_PIN 28
#define CASA_ASCII "\
         @@@@@@        %@@@@@@@  @&         @@@@@@#         ,@@@@@@@  @&\n\
     @@@@@@@@@     %@@@@@@@@@@@@@@@      @@@@@@@         @@@@@@@@@@@@@@@\n\
  &@@@@ / @@@    @@@@@    *@@@@@@        @@@@@&       @@@@@.    @@@@@@.\n\
@@@@@           @@@@@     @@@@@&      @@@     @@@@   @@@@      @@@@@@       /@@\n\
@@@@        /@@@@@@&     @@@@@@@     @@@@     &@@@@.@@@@     %@@@@@@&     %@@@\n\
@@@@.     @@@@@@@@/   .@@@@ @@@@( @@@@@@       @@@@@%@@@@@  @@@@ @@@@@ (@@@@\n\
@@@@@@@@@@@@/  *@@@@@@@@@   @@@@@@@@*@@@@@@@@@@@@@%  @@@@@@@@@,  @@@@@@@@&\n\
 @@@@@@@@@/     *@@@@@@@     @@@@@@/   *@@@@@@@@@/    @@@@@@      @@@@@@&\n\n"

bool is_night_time;
uint8_t rgb_gpio[3] = { 23, 24, 25 };

int server_socket;
int active_socket;

/// <summary>
/// Interrupt service routine for PIR sensors.
/// </summary>
void did_detect_motion_signal(void);

/// <summary>
/// Interrupt service routine for light sensors. Emits a daylight report to client and adjusts home lighting.
/// </summary>
void did_detect_daylight_change(void);

/// <summary>
/// Registers interrupts to home sensors.
/// </summary>
int arm_sensors(void);

/// <summary>
/// Populates home with rooms and nodes within.
/// </summary>
void populate_rooms(void);

/// <summary>
/// Evaluates the message from a client.
/// </summary>
/// <param name="client_socket">Active client socket.</param>
/// <param name="message">Message to evaluate.</param>
void evaluate_message(const int client_socket, const char *message);

/// <summary>
/// Main controller body. Receives and processes messages from socket connections.
/// </summary>
int run_server(void); // credit to: http://beej.us/guide/bgnet/html/single/bgnet.html

/// <summary>
/// Terminates a socket and its descriptor.
/// </summary>
/// <param name="client_socket">The client socket.</param>
/// <param name="active_fds">The active FDS.</param>
void shutdown_socket(int client_socket, fd_set *active_fds);

/// <summary>
/// Entry point.
/// </summary>
/// <param name="argc">Passed arg count</param>
/// <param name="argv">Passed arg vector</param>
/// <returns></returns>
int main(int argc, char *argv[]);