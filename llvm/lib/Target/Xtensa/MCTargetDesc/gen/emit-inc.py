import re, glob, sys, os

# Usage: emit-inc.py [<td-dir> <xt_table.tsv> <out.inc>]
# td-dir: the LLVM Xtensa target dir holding the *.td asm-string definitions.
_here   = os.path.dirname(os.path.abspath(__file__))
tddir   = sys.argv[1] if len(sys.argv) > 1 else os.path.normpath(_here + "/..")
tsvpath = sys.argv[2] if len(sys.argv) > 2 else "/tmp/xgen/xt_table.tsv"
outpath = sys.argv[3] if len(sys.argv) > 3 else "/tmp/xgen/ace15_enc.inc"
text = ""
for f in glob.glob(tddir+"/*.td"):
    text += open(f).read()+"\n"
# strip // comments
text = re.sub(r"//[^\n]*", "", text)

# join defs: capture def NAME : ...args... up to the "> {" or "> ;"
# operands: (outs ...) (ins ...) ; asm string with \t
defs = {}
# find "def NAME :" then take until the matching ">" that precedes "{" or ";" at top-ish level - approx: until "> {" or ">;" or "> ;"
for m in re.finditer(r"def\s+(AE_[A-Za-z0-9_]+)\s*:\s*(.*?)>\s*[{;]", text, re.S):
    name, body = m.group(1), m.group(2)
    outs = re.search(r"\(outs([^)]*)\)", body)
    ins  = re.search(r"\(ins([^)]*)\)", body)
    asm  = re.search(r"\"([a-z][a-zA-Z0-9_.]*)\\t([^\"]*)\"", body)
    if not asm: 
        continue
    mnem = asm.group(1)
    asmops = re.findall(r"\$([A-Za-z0-9_]+)", asm.group(2))
    mcops = []
    if outs: mcops += re.findall(r"\$([A-Za-z0-9_]+)", outs.group(1))
    if ins:  mcops += re.findall(r"\$([A-Za-z0-9_]+)", ins.group(1))
    defs[name] = (mnem, asmops, mcops)

# load xt table: mnem -> (len, tmpl, nops, [ (reg, [bits...]) ])
xt = {}
for line in open(tsvpath):
    p = line.rstrip("\n").split("\t")
    mnem, ln, tmpl, nops = p[0], int(p[1]), p[2], int(p[3])
    ops=[]
    for i in range(nops):
        seg = p[4+i]
        reg, bits = seg.split(":")
        bl = [int(x) for x in bits.split(",")] if bits else []
        ops.append((int(reg), bl))
    xt[mnem] = (ln, tmpl, nops, ops)

emitted=0; skipped=[]
out=open(outpath,"w")
out.write("// Auto-generated from ace15 xtensa-modules.c (libisa). Do not edit.\n")
out.write("// XTENSA_BUNDLE_ENC(LLVMName, len, {template bytes}, nOperands, {ops...})\n")
for name,(mnem,asmops,mcops) in sorted(defs.items()):
    if mnem not in xt: continue
    ln,tmpl,nops,ops = xt[mnem]
    if len(asmops) != nops:
        skipped.append((name,mnem,"asmops%d!=xtnops%d"%(len(asmops),nops))); continue
    # map each xtensa arg i (== asmops[i]) to MCInst index = position in mcops
    opdesc=[]
    ok=True
    for i in range(nops):
        an = asmops[i]
        if an not in mcops: ok=False; break
        mci = mcops.index(an)
        reg, bits = ops[i]
        opdesc.append((mci, reg, bits))
    if not ok:
        skipped.append((name,mnem,"asmop not in mcops")); continue
    if ln<=3: skipped.append((name,mnem,"standalone")); continue
    if any(reg==2 for (_,reg,_) in opdesc): skipped.append((name,mnem,"nonident-imm")); continue
    tb = ",".join("0x%s"%tmpl[j:j+2] for j in range(0,len(tmpl),2))
    opstr = ", ".join("{%d,%d,{%s},%d}"%(mci,reg,",".join(map(str,bits)),len(bits)) for (mci,reg,bits) in opdesc)
    out.write("XTENSA_BUNDLE_ENC(%s, %d, {%s}, %d, {%s})\n"%(name,ln,tb,nops,opstr))
    emitted+=1
out.close()
print("emitted", emitted, "skipped", len(skipped))
for s in skipped[:12]: print("  skip", s)
