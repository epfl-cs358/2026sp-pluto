import math
from ik_solver import IK as _solve_ik

COXA  = 0.39
FEMUR = 1.20 # upper leg
TIBIA = 1.36 # lower leg

FOOT_Z_STAND = -2.3


def _ik(x, y, z, right_side=False):
    coxa_sign = -1.0 if right_side else 1.0
    return _solve_ik(x, y, z, coxa_sign * COXA, FEMUR, TIBIA)


_RIGHT_LEGS = {"fr", "br"}


class BaseGait:
    swing_ratio = 0.35
    stand_hip = 0.0

    # From body z = 0.5, hu, hl = 0.3
    # foot_z = 0.5 - 0.3*cos(hip) - 0.3*cos(hip + knee) = 0
    stand_knee = 0.85
    
    stride = 0.8
    lift = 0.5 # extra knee bending at mid-swing to lift a foot

    def __init__(self, period):
        self.period  = period
        self.offsets = {}

    def _foot_from_phase(self, phase, forward_scale=1.0):
        if phase < self.swing_ratio:
            t = phase / self.swing_ratio
            x = (-self.stride + 2.0 * self.stride * t) * forward_scale
            z = FOOT_Z_STAND + self.lift * math.sin(math.pi * t)
        else:
            t = (phase - self.swing_ratio) / (1.0 - self.swing_ratio)
            x = (self.stride - 2.0 * self.stride * t) * forward_scale
            z = FOOT_Z_STAND
        return x, 0.0, z

    def get_phase(self, time):
        return (time % self.period) / self.period

    def leg_phase(self, time, leg):
        return (self.get_phase(time) + self.offsets[leg]) % 1.0

    def get_leg_angles(self, time, leg, forward_scale=1.0):
        phase = self.leg_phase(time, leg)
        x, y, z = self._foot_from_phase(phase, forward_scale)
        return _ik(x, y, z, right_side=leg in _RIGHT_LEGS)

    def stand_angles(self, right_side=False):
        return _ik(0.0, 0.0, FOOT_Z_STAND, right_side=right_side)


class Walk(BaseGait):
    def __init__(self, period):
        super().__init__(period)
        self.offsets = {
            "fl": 0.0,
            "bl": 0.25,
            "fr": 0.5,
            "br": 0.75,
        }


class Trot(BaseGait):
    def __init__(self, period):
        super().__init__(period)
        self.offsets = {
            "fl": 0.0,
            "fr": 0.5,
            "bl": 0.5,
            "br": 0.0,
        }


class Gallop(BaseGait):
    def __init__(self, period):
        super().__init__(period)
        self.offsets = {
            "fl": 0.0,
            "fr": 0.1,
            "br": 0.5,
            "bl": 0.6,
        }
