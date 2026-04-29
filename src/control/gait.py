import math

class BaseGait:
    swing_ratio = 0.35
    stand_hip = 0.0

    # From body z = 0.5, hu, hl = 0.3
    # foot_z = 0.5 - 0.3*cos(hip) - 0.3*cos(hip + knee) = 0
    stand_knee = 0.85
    
    stride = 1.4
    lift = 0.8 # extra knee bending at mid-swing to lift a foot

    def __init__(self, period):
        self.period = period
        self.offsets = {}

    def get_phase(self, time):
        return (time % self.period) / self.period

    def leg_phase(self, time, leg):
        return (self.get_phase(time) + self.offsets[leg]) % 1.0

    def get_leg_angles(self, time, leg, forward_scale=1.0):
        # forward_scale = hip movement
        phase = self.leg_phase(time, leg)
        hip_offset, knee = self._angles_from_phase(phase)
        return self.stand_hip + hip_offset * forward_scale, knee

    def _angles_from_phase(self, phase):
        if phase < self.swing_ratio:
            t = phase / self.swing_ratio
            hip = -self.stride + 2.0 * self.stride * t
            knee = self.stand_knee + self.lift * math.sin(math.pi * t)
        else:
            t = (phase - self.swing_ratio) / (1.0 - self.swing_ratio)
            hip = self.stride - 2.0 * self.stride * t
            knee = self.stand_knee # TODO: should be changed as well (probably after implementing IK)
        return hip, knee
    
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
