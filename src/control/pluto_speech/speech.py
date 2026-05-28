import json
import logging
from vosk import Model, KaldiRecognizer
import sounddevice as sd

from pluto_server.server import PlutoController
from pluto_server import message


def start_speech_engine(controller: PlutoController):
    """
    Background worker for speech recognition.
    """
    model = Model(lang="en-us")
    grammar = '["pluto sit", "pluto stop", "pluto give paw", "pluto flip", "pluto bow", "[unk]"]'
    rec = KaldiRecognizer(model, 16000, grammar)

    def audio_callback(indata, frames, time, status):
        if status:
            print(status)
        if rec.AcceptWaveform(bytes(indata)):
            result = json.loads(rec.Result())
            text = result.get("text", "")

            if text:
                logging.info(f"Speech Recognized: '{text}'")

            if "sit" in text:
                logging.info("Action Triggered: SIT")
                msg = message.create_behavior(message.MessageBehaviorKind.BEHAVIOR_SIT)
                controller.send_messages([msg])
            elif "paw" in text:
                logging.info("Action Triggered: GIVE_PAW")
                msg = message.create_behavior(
                    message.MessageBehaviorKind.BEHAVIOR_GIVE_PAW
                )
                controller.send_messages([msg])
            elif "bow" in text:
                logging.info("Action Triggered: BOW")
                msg = message.create_behavior(message.MessageBehaviorKind.BEHAVIOR_BOW)
                controller.send_messages([msg])
            elif "flip" in text:
                logging.info("Action Triggered: FLIP")
                msg = message.create_behavior(message.MessageBehaviorKind.BEHAVIOR_FLIP)
                controller.send_messages([msg])
            elif "stop" in text:
                logging.info("Action Triggered: STOP")
                msg = message.Message(
                    message.MessageFamilyKind.KIND_MOVE,
                    message.MessageMoveKind.MOVE_STOP_FOR,
                    0,
                )
                controller.send_messages([msg])

    with sd.RawInputStream(
        samplerate=16000,
        blocksize=8000,
        dtype="int16",
        channels=1,
        callback=audio_callback,
    ):
        while True:
            sd.sleep(100)
