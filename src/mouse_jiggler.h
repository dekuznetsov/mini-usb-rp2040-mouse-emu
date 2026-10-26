#ifndef MOUSE_JIGGLER_H
#define MOUSE_JIGGLER_H

// Periodically nudges the mouse cursor over USB HID so the host does not
// go idle (screen lock / "away" status).

// Non-blocking; call on every main loop iteration, after tud_task().
void mouse_jiggler_task(void);

#endif // MOUSE_JIGGLER_H
