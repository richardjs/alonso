import Board from "./Board.js";
import { e } from "./shortcuts.js";

function NewGameButton() {
  return e(
    "button",
    {
      className: "btn btn-danger",
      style: { width: "100%" },
      onClick: () => {
        if (confirm("New game?")) window.location = "";
      },
    },
    "New game",
  );
}

export default function App() {
  const [state, setState] = React.useState(
    location.hash.slice(1) || ".........................",
  );
  const [actions, setActions] = React.useState({});
  const [actionInput, setActionInput] = React.useState("");
  const [player, setPlayer] = React.useState(1);

  const [iterations, setIterations] = React.useState(
    localStorage.getItem("iterations") || 5000,
  );
  const [workers, setWorkers] = React.useState(
    localStorage.getItem("workers") || 1,
  );
  const [waitingForAI, setWaitingForAI] = React.useState(false);
  const [score, setScore] = React.useState("");

  function handleIterationsChange(e) {
    setIterations(e.target.value);
    localStorage.setItem("iterations", e.target.value);
  }

  function handleWorkersChange(e) {
    setWorkers(e.target.value);
    localStorage.setItem("workers", e.target.value);
  }

  React.useEffect(() => {
    function handleHashChange() {
      setState(location.hash.slice(1));
    }
    window.addEventListener("hashchange", handleHashChange);

    return () => {
      window.removeEventListener("hashchange", handleHashChange);
    };
  }, []);

  React.useEffect(() => {
    let ignore = false;

    function fetchActions() {
      fetch("/state/" + state + "/actions")
        .then((response) => response.json())
        .then((json) => {
          if (ignore) return;
          setActions(json.actions);
        });
    }

    function think() {
      fetch("/state/" + state + "/think", {
        headers: [
          ["Iterations", iterations],
          ["Workers", workers],
        ],
      })
        .then((response) => response.json())
        .then((json) => {
          if (ignore) return;
          setPlayer(player === 1 ? 2 : 1);
          location.hash = json.state;
          console.log(json.log);

          let logLines = json.log.split("\n");
          let score = "";
          for (let i = 0; i < logLines.length; i++) {
            let line = logLines[i];
            if (!line.startsWith("score")) continue;

            score = line.split("\t").at(-1);
            break;
          }

          setScore(score);
        });
    }

    if (player == 1) {
      fetchActions();
    } else {
      think();
    }

    return () => {
      ignore = true;
    };
  }, [state]);

  function handleActionInput(actionInputPart) {
    if (actionInputPart == actionInput) {
      setActionInput("");
      return;
    }

    const newActionInput = actionInput + actionInputPart;

    for (const action in actions) {
      if (action == newActionInput) {
        setPlayer(player === 1 ? 2 : 1);
        location.hash = actions[action];
        setActionInput("");
        break;
      } else if (action.startsWith(newActionInput)) {
        setActionInput(newActionInput);
        break;
      }
    }
  }

  return e(
    "div",
    null,
    e(Board, { state, player, actions, actionInput, handleActionInput }),
    e(AIOptions, {
      //waitingForLimits:
      //  this.props.max_iterations === null ||
      //  this.props.min_iterations === null ||
      //  this.props.max_workers === null ||
      //  this.props.min_workers === null,
      //max_iterations: this.state.max_iterations,
      //min_iterations: this.state.min_iterations,
      //max_workers: this.state.max_workers,
      //min_workers: this.state.min_workers,
      max_iterations: 100000,
      min_iterations: 5000,
      max_workers: 4,
      min_workers: 1,
      iterations: iterations,
      workers: workers,
      //player1_ai: this.state.player1_ai,
      //player2_ai: this.state.player2_ai,
      waitingForAI: waitingForAI,
      //waitingForHint: this.state.waitingForHint,
      //hint: this.state.hint,
      score: score,
      handleIterationsChange: handleIterationsChange,
      handleWorkersChange: handleWorkersChange,
      //handleHintClick: this.handleHintClick,
      //onPlayerAIChange: this.onPlayerAIChange,
    }),
    e(
      "div",
      { className: "row mt-4" },
      e("div", { className: "col ps-0" }, NewGameButton()),
    ),
  );
}
