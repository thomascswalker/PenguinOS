## Memory layout

This assumes running QEMU with `-m 512M` in order to emulate
512MB of RAM.

## Physical

| Purpose | Start | End | Size
| --- | --- | --- | ---
| Kernel | `0x0000001C` | `0x00000400` | `996B`
| Multiboot Header | `0x000100A4` | `0x00010134` | `144B`
| Heap | `0x00100000` | `0x1FFE0000` | `535MB`

## Virtual

| Purpose | Start | End | Size
| --- | --- | --- | ---
| Kernel | `0xC000001C` | `0xC0000400` | `996B`
| Multiboot Header | `0xC00100A4` | `0xC0010134` | `144B`
| Heap | `0xC0100000` | `0xE00E0000` | `535MB`
