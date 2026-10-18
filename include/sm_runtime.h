#ifndef SM_RUNTIME_H
#define SM_RUNTIME_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

#include "sm_limits.h"

// Install process signal handlers used for graceful shutdown.
void install_signal_handlers(void);
// Return process pid with the given name, or 0 if not found.
pid_t find_pid_by_name(const char *name, bool exclude_self);
// Return true when shutdown was requested by signal or kill file.
bool should_stop_requested(void);
// Request graceful shutdown with a descriptive source string.
void request_shutdown_stop(const char *reason);
// Return true when the payload is paused for system suspend/resume.
bool runtime_sleep_mode_active(void);
// Return true while a launch may briefly wait for USB storage after resume.
bool runtime_resume_grace_active(void);
// Enter or leave sleep mode with a descriptive source string.
// Returns true when the runtime sleep state changed.
bool request_runtime_sleep_mode(bool active, const char *reason);
// Serialize mount/link/cache publication against suspend cleanup.
void runtime_mount_state_lock(void);
void runtime_mount_state_unlock(void);
// Request an immediate scan cycle with a descriptive source string.
// Return false when rest mode prevents accepting the request.
bool request_scan_now(const char *reason);
// Request an immediate scan and optionally reset mount/install retry counters
// immediately before that scan starts.
bool request_scan_now_with_options(const char *reason, bool reset_attempts);
// Request an immediate scan on behalf of a running title. When that title is
// the active game, the scanner may run the scan without waiting for it to
// exit; otherwise this behaves like request_scan_now_with_options().
// requester_title_id may be NULL or empty for an ordinary request.
bool request_scan_now_for_title(const char *reason, bool reset_attempts,
                                const char *requester_title_id);
// Consume a pending immediate scan request and its merged options.
bool consume_scan_now_request(char *reason_out, size_t reason_out_size,
                              bool *reset_attempts_out);
// Copy the title ID that most recently requested the pending immediate scan.
// Returns false when no scan is pending or no requester title was given.
bool peek_scan_now_requester(char requester_title_id_out[MAX_TITLE_ID]);
// Consume the pending immediate scan only while its requester is still
// requester_title_id. Returns false and leaves the request queued otherwise.
bool consume_scan_now_request_for_requester(const char *requester_title_id,
                                            char *reason_out,
                                            size_t reason_out_size,
                                            bool *reset_attempts_out);
// Mark the title a requester scan must not mutate while it runs. NULL or an
// empty string clears the guard after that scan.
void runtime_set_scan_protected_title(const char *title_id);
// Return true when the running scan must leave this title alone: either the
// requester title itself, or any title ShellCore currently has prepared.
// Always false outside a requester scan.
bool runtime_scan_title_is_protected(const char *title_id);
// Sleep in chunks and stop early if shutdown was requested.
bool sleep_with_stop_check(unsigned int total_us);

#endif
