import re
import sys

def translate_line(line):
    line = line.strip()
    if not line:
        return ""
    
    # Keep labels and section entries if they are part of functions
    if line.startswith(".") and not line.startswith(".L"):
        # We can pass through globl for our entry points, but strip cfi/align
        if line.startswith(".globl") or line.startswith(".global"):
            parts = line.split()
            sym = parts[1]
            if sym == "eval":
                sym = "_eval"
            return f"global {sym}"
        return ""
    
    # Handle labels
    if line.endswith(":"):
        if line == "eval:":
            return "_eval:"
        return line
    
    # Split opcode and operands
    parts = re.split(r'\s+', line, 1)
    opcode = parts[0]
    operands_str = parts[1] if len(parts) > 1 else ""
    
    # Strip comments if any
    if "#" in operands_str:
        operands_str = operands_str.split("#")[0].strip()
        
    # AT&T operands are separated by commas, but we must protect internal commas in (reg,reg,scale)
    # Let's parse memory arguments first
    operands = []
    # Simple brackets balance parser for commas
    curr_op = ""
    in_paren = False
    for char in operands_str:
        if char == '(':
            in_paren = True
            curr_op += char
        elif char == ')':
            in_paren = False
            curr_op += char
        elif char == ',' and not in_paren:
            operands.append(curr_op.strip())
            curr_op = ""
        else:
            curr_op += char
    if curr_op.strip():
        operands.append(curr_op.strip())
        
    # Translate operands
    for idx, op in enumerate(operands):
        # Match memory locations like offset(base, index, scale) or (base)
        mem_match = re.match(r'^([\-0-9]*)\((%[a-z0-9]+)(?:,(%[a-z0-9]+))?(?:,([0-9]+))?\)$', op)
        if mem_match:
            offset = mem_match.group(1) or "0"
            base = mem_match.group(2).replace("%", "")
            index = mem_match.group(3)
            scale = mem_match.group(4)
            
            # Adjust stack offsets for 16-bit argument structures
            # We pushed 4 dwords (16 bytes) in eval start, plus 2 bytes return address.
            # Total 18 bytes offset.
            # 20(%esp) -> [bp + 10] -> [esp + 18]
            # 24(%esp) -> [bp + 12] -> [esp + 20]
            if base == "esp":
                if offset == "20":
                    offset = "18"
                elif offset == "24":
                    offset = "22"
                    
            intel_mem = f"[{base}"
            if index:
                index = index.replace("%", "")
                scale_val = f"*{scale}" if scale else ""
                intel_mem += f" + {index}{scale_val}"
            if offset != "0":
                intel_mem += f" + {offset}"
            intel_mem += "]"
            operands[idx] = intel_mem
        else:
            # Strip standard register designators or immediate signs
            operands[idx] = op.replace("%", "")
            if operands[idx].startswith("$"):
                operands[idx] = operands[idx][1:]
                
    # Swap operand order (AT&T is src, dest -> Intel is dest, src)
    if len(operands) == 2:
        operands = [operands[1], operands[0]]
        
    # Match Opcode translations BEFORE general suffix stripping
    intel_opcode = opcode
    if opcode.startswith("j"):
        # Protect jump instructions from suffix stripping
        pass
    elif opcode in ["movzbl", "movzbg", "movzb"]:
        intel_opcode = "movzx"
        if len(operands) == 2 and "[" in operands[1]:
            operands[1] = "byte " + operands[1]
    elif opcode in ["movzwl", "movzwb", "movzw"]:
        intel_opcode = "movzx"
        if len(operands) == 2 and "[" in operands[1]:
            operands[1] = "word " + operands[1]
    elif opcode == "testw":
        intel_opcode = "test"
    elif opcode == "cmpb":
        intel_opcode = "cmp"
    elif opcode == "andb":
        intel_opcode = "and"
    elif opcode == "addb":
        intel_opcode = "add"
    elif opcode == "subb":
        intel_opcode = "sub"
    elif opcode in ["incl", "incw", "incb"]:
        intel_opcode = "inc"
    elif opcode in ["decl", "decw", "decb"]:
        intel_opcode = "dec"
    elif opcode == "leal":
        intel_opcode = "lea"
    elif opcode == "retl":
        intel_opcode = "ret"
    # General suffix stripping for math operations
    elif opcode.endswith("l") and opcode not in ["call", "shl", "shld"]:
        intel_opcode = opcode[:-1]
    elif opcode.endswith("b") and opcode not in ["sub"]:
        intel_opcode = opcode[:-1]
    elif opcode.endswith("w"):
        intel_opcode = opcode[:-1]
        
    # Check for short jumps which in NASM don't need additional constraints, 
    # but handle direct branches
    if intel_opcode.startswith("j") and len(operands) == 1:
        # e.g. jne .LBB1_16
        pass
        
    operands_str_out = ", ".join(operands)
    return f"    {intel_opcode} {operands_str_out}".strip()

def main():
    if len(sys.argv) < 3:
        print("Usage: python3 translate.py <input.s> <output.asm>")
        sys.exit(1)
        
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    with open(input_file, "r") as f:
        lines = f.readlines()
        
    out_lines = [
        ";; Translated from 32-bit i686 AT&T asm to 16-bit 386 Intel asm",
        "BITS 16",
        "SECTION _TEXT class=CODE",
        ""
    ]
    
    for line in lines:
        tr = translate_line(line)
        if tr:
            out_lines.append(tr)
            
    with open(output_file, "w") as f:
        f.write("\n".join(out_lines) + "\n")
    print(f"Translation complete! Saved to {output_file}")

if __name__ == "__main__":
    main()
