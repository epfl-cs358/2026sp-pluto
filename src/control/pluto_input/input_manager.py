import math


class InputManager:
    def __init__(self, deadzone=0.15):
        self.deadzone = deadzone
        self._keyboard_vector = [0.0, 0.0]
        self._gamepad_vector = [0.0, 0.0]

    def update_from_keyboard(self, w: bool, a: bool, s: bool, d: bool):
        x, y = 0.0, 0.0
        if d:
            x += 1.0
        if a:
            x -= 1.0
        if w:
            y += 1.0
        if s:
            y -= 1.0
        self._keyboard_vector = [x, y]

    def update_from_gamepad(self, axis_x: float, axis_y: float):
        x = axis_x if abs(axis_x) > self.deadzone else 0.0
        y = axis_y if abs(axis_y) > self.deadzone else 0.0
        self._gamepad_vector = [x, -y]

    def get_movement_vector(self) -> tuple[float, float]:
        raw_x = self._keyboard_vector[0] + self._gamepad_vector[0]
        raw_y = self._keyboard_vector[1] + self._gamepad_vector[1]

        magnitude = math.sqrt(raw_x**2 + raw_y**2)
        if magnitude == 0:
            return (0.0, 0.0)
        if magnitude > 1.0:
            return (raw_x / magnitude, raw_y / magnitude)

        return (raw_x, raw_y)
