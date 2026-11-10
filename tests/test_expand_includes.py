import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest


EXPANDER = Path(__file__).resolve().parents[1] / "tools" / "expand_includes.py"


class ExpandIncludesTest(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.workspace = Path(self.temp.name)
        self.repo = self.workspace / "custom_lib"
        self.script = self.repo / "tools" / "expand_includes.py"
        self.script.parent.mkdir(parents=True)
        shutil.copyfile(EXPANDER, self.script)
        self.source = self.workspace / "solutions" / "main.cpp"
        self.cwd = self.workspace / "unrelated"
        self.cwd.mkdir()
        self.write("custom_lib/libraries/graph/Top.hpp", (
            '#pragma once\n#include "libraries/utility/Middle.hpp"\n'
        ))
        self.write("custom_lib/libraries/utility/Middle.hpp", (
            '#pragma once\n#include "Leaf.hpp"\n'
        ))
        self.write("custom_lib/libraries/utility/Leaf.hpp", (
            '#pragma once\n#include <vector>\nconstexpr int library_value = 42;\n'
        ))

    def write(self, name, contents):
        path = self.workspace / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(contents)
        return path

    def expand(self, contents, *args, cwd=None, env=None):
        self.source.parent.mkdir(parents=True, exist_ok=True)
        self.source.write_text(contents)
        environment = dict(os.environ, CPLUS_INCLUDE_PATH="")
        if env:
            environment.update(env)
        result = subprocess.run(
            [sys.executable, str(self.script), str(self.source), "--stdout", *args],
            cwd=cwd or self.cwd,
            env=environment,
            capture_output=True,
            text=True,
            check=True,
        )
        return result.stdout

    def test_recursive_includes_outside_repository(self):
        header = self.repo / "libraries" / "graph" / "Top.hpp"
        for name in (
            "libraries/graph/Top.hpp",
            "graph/Top.hpp",
            "../custom_lib/libraries/graph/Top.hpp",
            str(header),
        ):
            with self.subTest(include=name):
                output = self.expand(
                    f'#include "{name}"\n'
                    '#include "libraries/utility/Leaf.hpp"\nint main() {}\n'
                )
                self.assertEqual(output.count("constexpr int library_value = 42;"), 1)
                self.assertNotIn('#include "', output)
                self.assertNotIn("#pragma once", output)
                self.assertIn("#include <vector>", output)

    def test_workspace_prefixed_include(self):
        output = self.expand(
            '#include "custom_lib/libraries/graph/Top.hpp"\n',
            cwd=self.workspace,
        )
        self.assertIn("constexpr int library_value = 42;", output)
        self.assertNotIn('#include "', output)

    def test_including_header_directory_takes_priority(self):
        self.write("solutions/Leaf.hpp", "wrong_leaf\n")
        output = self.expand('#include "libraries/graph/Top.hpp"\n')
        self.assertIn("constexpr int library_value = 42;", output)
        self.assertNotIn("wrong_leaf", output)

    def test_explicit_include_directory(self):
        self.write("extra/Custom.hpp", '#include "libraries/graph/Top.hpp"\n')
        output = self.expand('#include "Custom.hpp"\n', "-I", str(self.workspace / "extra"))
        self.assertIn("constexpr int library_value = 42;", output)
        self.assertNotIn('#include "', output)

    def test_environment_include_directory(self):
        self.write("extra/Custom.hpp", '#include "libraries/graph/Top.hpp"\n')
        output = self.expand(
            '#include "Custom.hpp"\n',
            env={"CPLUS_INCLUDE_PATH": str(self.workspace / "extra")},
        )
        self.assertIn("constexpr int library_value = 42;", output)
        self.assertNotIn('#include "', output)

    def test_acl_inside_repository_or_sibling(self):
        for name in ("custom_lib/ac-library", "ac-library"):
            with self.subTest(acl_location=name):
                acl = self.workspace / name
                self.write(f"{name}/atcoder/segtree", '#include "atcoder/internal_bit"\n')
                self.write(f"{name}/atcoder/internal_bit.hpp", "constexpr int acl_value = 7;\n")
                output = self.expand('#include <atcoder/segtree>\n')
                self.assertIn("constexpr int acl_value = 7;", output)
                self.assertNotIn("#include", output)
                output = self.expand('#include <atcoder/segtree>\n', "--no-expand-acl")
                self.assertEqual(output, '#include <atcoder/segtree>\n')
                shutil.rmtree(acl)

    def test_output_file(self):
        self.source.parent.mkdir(parents=True)
        self.source.write_text('#include "libraries/graph/Top.hpp"\n')
        output = self.cwd / "output" / "combined.cpp"
        subprocess.run(
            [sys.executable, str(self.script), str(self.source), "-o", str(output)],
            cwd=self.cwd,
            env=dict(os.environ, CPLUS_INCLUDE_PATH=""),
            capture_output=True,
            text=True,
            check=True,
        )
        self.assertIn("constexpr int library_value = 42;", output.read_text())


if __name__ == "__main__":
    unittest.main()
