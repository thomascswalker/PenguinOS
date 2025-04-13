import logging
import argparse
import os
import subprocess
import sys

# Globals

OS_NAME = "PengOS"

PATH = "~/opt/cc_i386/bin"  # os.path.expanduser("~/opt/cc_i386/bin")
ARCH = "i386"
HOST = "i686-elf"
GCC = os.path.join(PATH, f"{HOST}-g++")
AR = os.path.join(PATH, f"{HOST}-ar")
LD = os.path.join(PATH, f"{HOST}-ld")
AS = "nasm"

SOURCE_DIR = "./source"
BUILD_DIR = "./build"

KERNEL_FILE = os.path.join(BUILD_DIR, f"boot/{OS_NAME}.bin")
"""The kernel binary."""
ISO_FILE = f"{BUILD_DIR}/boot/{OS_NAME}.iso"
"""The ISO image of the OS."""
DISK_FILE = f"{BUILD_DIR}/disk.img"

OPTIMIZE = "-O0"
DEBUG = "-g"
STD_VERSION = "-std=c++23"
GCC_FLAGS = f"-m32 {DEBUG} {OPTIMIZE} -ffreestanding -Wall -fno-omit-frame-pointer -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -Wno-parentheses -fno-exceptions"
NASM_FLAGS = f"-felf32 {DEBUG}"
LD_FLAGS = "-m elf_i386"

HDD_SIZE = 100
MEM_SIZE = 32

# Setup the logger


class ColoredFormatter(logging.Formatter):
    def format(self, record):
        if record.levelno == logging.DEBUG:
            record.msg = "\033[3m%s\033[0m" % record.msg
        elif record.levelno == logging.INFO:
            record.msg = "\033[1m%s\033[0m" % record.msg
        elif record.levelno == logging.WARNING:
            record.msg = "\033[93m%s\033[0m" % record.msg
        elif record.levelno == logging.ERROR:
            record.msg = "\033[91m%s\033[0m" % record.msg
        return logging.Formatter.format(self, record)


logger = logging.getLogger(f"{OS_NAME}Build")
hdlr = logging.StreamHandler(sys.stdout)
hdlr.setFormatter(ColoredFormatter(fmt="%(asctime)s - %(levelname)s - %(message)s"))
logger.addHandler(hdlr)
logger.setLevel(logging.INFO)


def verify_file(file: str) -> None:
    if not os.path.exists(file):
        logger.error(f"File not found: {file}")
        sys.exit(1)


def get_files_of_type(root: str, file_type: str) -> list:
    files = (
        subprocess.check_output(
            f"find {root} -type f -name '*.{file_type}'", shell=True
        )
        .decode()
        .splitlines()
    )
    return files


def get_programs() -> list:
    programs = os.listdir("./programs/")
    root = os.path.dirname(__file__)
    return [p for p in programs if os.path.isdir(os.path.join(root, "programs", p))]


def run_cmd(command: str) -> str:
    logger.debug(f"Running command: {command}")
    result = subprocess.run(command, shell=True, text=True)
    if result.returncode != 0:
        logger.error(f"Command failed: {command}")
        sys.exit(1)
    return result


class CLI:
    _debug: bool = False
    _clean: bool = False
    object_files: list[str] = []

    def __init__(self):
        self.parser = argparse.ArgumentParser(description=f"{OS_NAME} Build Script")

    def add_argument(self, *args, **kwargs) -> None:
        self.parser.add_argument(*args, **kwargs)

    def run(self) -> None:
        args = self.parser.parse_args()

        logger.info(f"Launching {OS_NAME} with arguments: {args}")

        if args.debug:
            self._debug = True
            logger.setLevel(logging.DEBUG)
            logger.debug("Debug mode enabled.")

        self._debug = True if args.debug else False
        self._clean = True if args.clean else False

        if args.rebuild:
            logger.info("Building kernel...")
            self.all()
            logger.info("Kernel build complete.")

        if self._clean:
            logger.info("Cleaning up files...")
            self.cleanup()
            logger.info("Cleanup complete.")

        logger.warning("Launching QEMU...")
        cmd = (
            "qemu-system-i386 "
            f"-m {MEM_SIZE}M "
            f"-cdrom {ISO_FILE} "
            "-boot d "
            f"-drive file={DISK_FILE},format=raw,if=ide "
            "-display gtk,zoom-to-fit=on"
        )
        run_cmd(cmd)

    def construct_build_dirs(self):
        if os.path.exists("build"):
            logger.debug("Build directory already exists. Cleaning up...")
            os.system("rm -rf build")
        logger.debug("Creating build directory...")
        os.makedirs(BUILD_DIR, exist_ok=False)
        os.makedirs(f"{BUILD_DIR}/boot/grub", exist_ok=True)
        logger.debug("Build directory created.")

    def get_include_arg(self) -> str:
        include_dirs = (
            subprocess.check_output("find ./source -type d", shell=True)
            .decode()
            .splitlines()
        )
        include_args = [f"-I{d}" for d in include_dirs if os.path.isdir(d)]
        return " ".join(include_args)

    def assemble(self, file: str) -> str:
        basename = os.path.splitext(os.path.basename(file))[0]
        output_file = os.path.join(BUILD_DIR, basename + "_s.o")
        logger.debug(f"Assembling {file} to {output_file}...")
        cmd = f"{AS} {NASM_FLAGS} {file} -o {output_file}"
        run_cmd(cmd)
        verify_file(output_file)
        return output_file

    def compile(self, file: str, include: bool = True) -> str:
        basename = os.path.splitext(os.path.basename(file))[0]
        output_file = os.path.join(BUILD_DIR, basename + "_cpp.o")
        include_arg = self.get_include_arg()
        cmd = (
            f"{GCC} {STD_VERSION} {GCC_FLAGS} -nostdlib -fno-builtin "
            f"{include_arg if include else ''} -c {file} -o {output_file}"
        )
        run_cmd(cmd)
        verify_file(output_file)
        return output_file

    def build_bin(self, name: str, object_files: list[str]) -> str:
        logger.debug("Linking object files...")
        kernel_file = os.path.join(BUILD_DIR, f"boot/{name}.bin")
        cmd = (
            f"{LD} {LD_FLAGS} -T {SOURCE_DIR}/arch/{ARCH}/linker.ld "
            f"{' '.join(object_files)} -o {kernel_file} "
        )
        run_cmd(cmd)
        verify_file(kernel_file)
        logger.debug("Linking complete.")
        return kernel_file

    def build_lib(self, name: str, object_files: list[str]) -> str:
        logger.debug(f"Creating library {name}...")
        lib_file = os.path.join(BUILD_DIR, f"{name}.a")
        cmd = f"{AR} r {lib_file} {' '.join(object_files)}"
        run_cmd(cmd)
        verify_file(lib_file)
        run_cmd(f"ranlib {lib_file}")
        logger.debug(f"Library {name} created.")
        return lib_file

    def build_program(self, directory: str):
        program_name = os.path.basename(directory)
        logger.debug(f"Building program {program_name}...")

        obj_files = []
        for f in get_files_of_type(directory, "cpp"):
            basename = os.path.splitext(os.path.basename(f))[0]
            obj_file = os.path.join(BUILD_DIR, basename + "_cpp.o")
            include_arg = "-I./source/libc -I./source/libcpp"
            cmd = (
                f"{GCC} {STD_VERSION} {GCC_FLAGS} -nostdlib -fno-builtin "
                f"{include_arg} -L./build -c {f} -o {obj_file} -l:libc.a"
            )
            run_cmd(cmd)
            verify_file(obj_file)

            obj_files.append(obj_file)

        logger.info(obj_files)
        bin_file = os.path.join(BUILD_DIR, f"{program_name}.bin")
        cmd = (
            f"{LD} {LD_FLAGS} -T ./programs/linker.ld "
            f"-L./build "
            f"{' '.join(obj_files)} -o {bin_file} -l:libc.a "
        )
        result = run_cmd(cmd)
        logger.info(f"Result: {result}")

        run_cmd(f"cp {bin_file} ./base/")

    def build_cfg(self):
        """Creates the GRUB configuration file."""
        grub_cfg = os.path.join(BUILD_DIR, "boot/grub/grub.cfg")
        logger.debug(f"Creating GRUB configuration at {grub_cfg}...")
        kernel_filename = os.path.basename(KERNEL_FILE)
        with open(grub_cfg, "w") as f:
            f.write(
                f'menuentry "{OS_NAME}" {{\n    multiboot /boot/{kernel_filename}\n}}\n'
            )
        verify_file(grub_cfg)
        logger.debug("GRUB configuration created.")

    def build_iso(self):
        logger.debug("Creating ISO image...")
        command = f"grub-mkrescue -o {ISO_FILE} build"
        run_cmd(command)
        verify_file(ISO_FILE)
        logger.debug("ISO image created.")

    def build_disk(self) -> None:
        logger.debug("Creating disk image...")
        cmds = [
            f"dd if=/dev/zero of={DISK_FILE} bs={HDD_SIZE}M count=1 status={'noxfer' if self._debug else 'none'}",
            f"mformat -i {DISK_FILE} -F",
            f"mcopy -i {DISK_FILE} -s ./base/ ::/",
            f"mdir -i {DISK_FILE} -/",
        ]

        for cmd in cmds:
            run_cmd(cmd)

        verify_file(DISK_FILE)
        logger.debug("Disk image created.")

    def cleanup(self) -> None:
        for file in self.object_files:
            run_cmd(f"rm -f {file}")

    def build_std_lib(self):
        dirs = ["./source/libc", "./source/libcpp"]
        self.libraries = []
        obj_files = []
        for d in dirs:
            files = []
            files += get_files_of_type(d, "cpp")
            for f in files:
                out_file = self.compile(f)
                obj_files.append(out_file)

        lib_name = "libc"
        logger.debug(f"Building {lib_name} with files: {obj_files}")
        lib_file = self.build_lib(lib_name, obj_files)
        logger.debug(f"Built library file: {lib_file}")
        verify_file(lib_file)
        self.libraries.append(lib_file)

        if self._clean:
            for f in obj_files:
                run_cmd(f"rm -f {f}")

    def build_kernel(self):
        # Assmemble .s files
        obj_files = []
        for asm in get_files_of_type("./source", "s"):
            logger.debug(f"Processing assembly file: {asm}")
            obj_files.append(self.assemble(asm))

        # Compile .cpp files
        for cpp in get_files_of_type("./source", "cpp"):
            logger.debug(f"Processing C++ file: {cpp}")
            obj_files.append(self.compile(cpp))

        logger.info(f"Object files: {obj_files}")

        kernel = self.build_bin(OS_NAME, obj_files)
        assert kernel == KERNEL_FILE, "Kernel file mismatch."
        logger.info(f"Kernel linked to: {KERNEL_FILE}")

        if self._clean:
            for f in obj_files:
                run_cmd(f"rm -f {f}")

    def all(self) -> None:
        self.construct_build_dirs()
        self.build_std_lib()
        self.build_kernel()

        logger.info("Building programs...")
        for d in get_programs():
            self.build_program(f"./programs/{d}")

        self.build_cfg()
        self.build_iso()
        self.build_disk()


cli = CLI()
cli.add_argument(
    "-d",
    "--debug",
    action="store_true",
    help="Enable debug mode",
)
cli.add_argument(
    "-c",
    "--clean",
    action="store_true",
    help="Clean up output files.",
)
cli.add_argument(
    "-r",
    "--rebuild",
    default=False,
    action="store_true",
    help="Rebuilds the kernel.",
)
cli.run()
