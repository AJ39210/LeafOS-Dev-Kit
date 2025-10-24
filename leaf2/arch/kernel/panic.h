#ifndef PANIC_H
#define PANIC_H

// Trigger kernel panic with a reason
void kernel_panic(const char* reason);

// Check if testing mode is enabled
int panic_is_test_mode(void);

#endif