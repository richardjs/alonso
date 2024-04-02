from os import environ
from subprocess import run
from typing import Annotated

from fastapi import FastAPI, Header, HTTPException, Path
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel


WORKERS = 4
ITERATIONS = 1000

MAX_ITERATIONS = 50000
MIN_ITERATIONS = 100

MAX_WORKERS = 4
MIN_WORKERS = 1

ALONSO = environ.get("ALONSO", "./engine/alonso")


STATE_REGEX = r"^([xo.]){1,25}$"


app = FastAPI()


class EngineResponse(BaseModel):
    log: str


class ActionsResponse(EngineResponse):
    actions: dict[str, str]


class ThinkResponse(ActionsResponse):
    state: str


def alonso(*args) -> (str, str):
    p = run([ALONSO] + list(args), capture_output=True, encoding="utf-8")

    if p.returncode:
        raise HTTPException(status_code=422, detail=p.stderr)

    return p.stdout, p.stderr


def get_actions(state: str) -> (list[str], str):
    stdout, stderr = alonso("-l", state)
    if stdout == "":
        return ([], stderr)
    return (stdout.strip().split("\n"), stderr)


@app.get("/state/{state}/actions", response_model=ActionsResponse)
async def state_actions(state: str = Path(pattern=STATE_REGEX)) -> ActionsResponse:
    actions, stderr = get_actions(state)
    action_states = {
        action: alonso("-a", action, state)[0].strip() for action in actions
    }
    return ActionsResponse(actions=action_states, log=stderr)


@app.get("/state/{state}/think", response_model=ThinkResponse)
async def state_think(
    state: str = Path(pattern=STATE_REGEX),
    workers: Annotated[str | None, Header()] = None,
    iterations: Annotated[str | None, Header()] = None,
) -> ThinkResponse:

    if workers:
        workers = min(int(workers), MAX_WORKERS)
        workers = max(workers, MIN_WORKERS)

    if iterations:
        iterations = min(int(iterations), MAX_ITERATIONS)
        iterations = max(iterations, MIN_ITERATIONS)

    new_state, stderr = alonso(f"-t", "-w", str(workers), "-i", str(iterations), state)
    actions, _ = get_actions(new_state)
    action_states = {
        action: alonso("-a", action, new_state)[0].strip() for action in actions
    }
    return ThinkResponse(state=new_state, actions=action_states, log=stderr)


app.mount("/", StaticFiles(directory="ui", html=True))
