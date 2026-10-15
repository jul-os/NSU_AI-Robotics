import io
import re
import sys
import warnings
from pathlib import Path

warnings.filterwarnings("ignore")

BASE_DIR = Path(__file__).resolve().parent
DATA_FILE = BASE_DIR / "archive" / "WineQT.csv"
if str(BASE_DIR) not in sys.path:
    sys.path.insert(0, str(BASE_DIR))

try:
    from IPython.display import display
except Exception:
    def display(*args, **kwargs):
        for obj in args:
            print(obj)


_SRC = (BASE_DIR / "lab2.py").read_text(encoding="utf-8")
_lines = _SRC.splitlines(keepends=True)
stains = [i for i, line in enumerate(_lines) if line.startswith("# ЗАДАНИЕ [")]
_starts = [1] + stains + [len(_lines) + 1]
_names = [
    "setup", "task_2_3", "task_2_4", "task_2_5",
    "task_2_6", "task_2_7", "task_2_8", "task_2_9",
    "task_2_10", "task_2_11", "task_2_12",
]
_TASKS = {
    name: "".join(_lines[a - 1:b - 1])
    for name, a, b in zip(_names, _starts, _starts[1:])
}

_STATE = None
_COMPLETED = set()
_SHOWN_FIGURES = set()


def _state():
    global _STATE
    if _STATE is None:
        _STATE = {"__name__": "__main__", "__file__": str(BASE_DIR / "lab2.py")}
    return _STATE


_ORDER = list(_TASKS)


def _vis_key(task_name):
    return "VIS_" + task_name.split("task", 1)[1].replace("_", "")


def _merge_vis_defaults(code):
    def _repl(match):
        name = match.group(1)
        body = match.group(2)
        return (
            f"_SAVED_VIS_{name} = globals().get('VIS_{name}') "
            f"if isinstance(globals().get('VIS_{name}'), dict) else None\n"
            f"VIS_{name} = {{\n{body}}}\n"
            f"if isinstance(_SAVED_VIS_{name}, dict):\n"
            f"    VIS_{name}.update(_SAVED_VIS_{name})\n"
        )

    return re.sub(
        r"^VIS_(\w+) = \{\n(.*?)^\}\s*\n",
        _repl,
        code,
        flags=re.MULTILINE | re.DOTALL,
    )


def _run_one(task_name, settings=None, suppress=True, force=False):
    ns = _state()
    if task_name not in _ORDER:
        raise ValueError(f"Unknown task {task_name!r}")
    for prerequisite in _ORDER[:_ORDER.index(task_name)]:
        if prerequisite not in _COMPLETED:
            _run_one(prerequisite, None, suppress=suppress)
    already = task_name in _COMPLETED
    if already and settings is None and not force:
        return ns

    if settings is not None:
        ns[_vis_key(task_name)] = settings
    import matplotlib.pyplot as _plt

    code = _merge_vis_defaults(_TASKS[task_name])
    if task_name != "task_2_7":
        _plt.show = lambda *a, **k: None

    with io.StringIO() as _buf:
        _old_stdout = sys.stdout
        if suppress:
            sys.stdout = _buf
        try:
            exec(compile(code, task_name, "exec"), ns)
        finally:
            sys.stdout = _old_stdout

    _COMPLETED.add(task_name)
    return ns


TABLE_VARS = {
    "setup": ["class_counts", "types_df", "nan_by_class", "zero_by_class", "desc_stats"],
    "task_2_5": [],
    "task_2_9": ["table29"],
    "task_2_10": ["table210"],
    "task_2_11": ["table211", "summary211"],
}


def run_task(task_name, settings=None, show_tables=True, force=False):
    try:
        import matplotlib.pyplot as _plt
        _pre_figures = set(_plt.get_fignums())
    except Exception:
        _pre_figures = set()

    ns = _run_one(task_name, settings=settings, suppress=True, force=force)

    if task_name == "task_2_7":
        try:
            _fig = ns.get("fig27")
            if _fig is not None:
                display(_fig)
        except Exception:
            pass

    if task_name != "task_2_7":
        try:
            import matplotlib.pyplot as _plt
            for _num in sorted(set(_plt.get_fignums()) - _pre_figures):
                display(_plt.figure(_num))
        except Exception:
            pass

    if show_tables and task_name in TABLE_VARS:
        for name in TABLE_VARS[task_name]:
            if name in ns:
                display(ns[name])
    return ns
