from os import environ
from subprocess import run

from fastapi import FastAPI, HTTPException, Path
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel


WORKERS = 4
ITERATIONS = 10000


ALONSO = environ.get("ALONSO", "./engine/alonso")


STATE_REGEX = r"^([xo.]){1,25}$"


app = FastAPI()


class EngineResponse(BaseModel):
    log: str


class ActionsResponse(EngineResponse):
    actions: dict[str, str]


def alonso(*args) -> (str, str):
    p = run([ALONSO] + list(args), capture_output=True, encoding="utf-8")

    if p.returncode:
        raise HTTPException(status_code=422, detail=p.stderr)

    return p.stdout, p.stderr


def get_actions(state: str) -> (list[str], str):
    stdout, stderr = alonso("-l", state)
    return (stdout.strip().split("\n"), stderr)


@app.get("/state/{state}/actions", response_model=ActionsResponse)
async def state_actions(state: str = Path(pattern=STATE_REGEX)) -> ActionsResponse:
    actions, stderr = get_actions(state)
    action_states = {
        action: alonso("-a", action, state)[0].strip() for action in actions
    }
    return ActionsResponse(actions=action_states, log=stderr)


app.mount("/", StaticFiles(directory="ui", html=True))
