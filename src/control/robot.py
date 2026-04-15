from dataclasses import dataclass
from leg import Leg

@dataclass
class Robot:
    """
    Represents the full quadruped robot.

    The robot is composed of 4 legs:
    - front_left
    - front_right
    - back_left
    - back_right

    Each leg contains 3 servos (hip, upper, lower).
    """
    
    # Each attribute is a Leg
    front_left: Leg
    front_right: Leg
    back_left: Leg
    back_right: Leg

    def stand(self) -> None:
        """
        Set all legs to a neutral standing position.

        We choose 90 degrees for all joints as a default
        """
        self.front_left.set_angles(90, 90, 90)
        self.front_right.set_angles(90, 90, 90)
        self.back_left.set_angles(90, 90, 90)
        self.back_right.set_angles(90, 90, 90)

    def print_state(self) -> None:
        """
        Print the current angles of each legs.

        For febugging
        """
        print("Front Left:", self.front_left.get_angles())
        print("Front Right:", self.front_right.get_angles())
        print("Back Left:", self.back_left.get_angles())
        print("Back Right:", self.back_right.get_angles())