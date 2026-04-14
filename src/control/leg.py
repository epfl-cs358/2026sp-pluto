class Leg:
    """
    Represents one robot leg with 3 servos:
    - hip
    - upper leg
    - lower leg
    """
#we can change the names
    def __init__(self, hip_servo, upper_servo, lower_servo):
        self.hip = hip_servo
        self.upper = upper_servo
        self.lower = lower_servo

    def set_angles(self, hip_angle, upper_angle, lower_angle):
        """Set all 3 joint angles"""
        self.hip.set_angle(hip_angle)
        self.upper.set_angle(upper_angle)
        self.lower.set_angle(lower_angle)

    def get_angles(self):
        """Return current angles"""
        return (
            self.hip.get_angle(),
            self.upper.get_angle(),
            self.lower.get_angle()
        )