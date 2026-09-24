INSTR_ENCS = {
  bdep: {
    opcode: { val: 0b011110, width: 6, offst: 0 },
    operands: {
      rd:  { width: 5, offst: 21, type: :register },
      rs1: { width: 5, offst: 16, type: :register },
      rs2: { width: 5, offst: 11, type: :register }
    }
  },

  nor: {
    opcode: { val: 0b001010, width: 6, offst: 0 },
    operands: {
      rd: { width: 5, offst: 11, type: :register },
      rs: { width: 5, offst: 21, type: :register },
      rt: { width: 5, offst: 16, type: :register }
    }
  },

  cls: {
    opcode: { val: 0b110111, width: 6, offst: 0 },
    operands: {
      rd: { width: 5, offst: 21, type: :register },
      rs: { width: 5, offst: 16, type: :register }
    }
  },

  syscall: {
    opcode: { val: 0b100011, width: 6, offst: 0 },
    operands: {
    }
  },

  add: {
    opcode: { val: 0b011011, width: 6, offst: 0 },
    operands: {
      rd: { width: 5, offst: 11, type: :register },
      rs: { width: 5, offst: 21, type: :register },
      rt: { width: 5, offst: 16, type: :register }
    }
  },

  ssat: {
    opcode: { val: 0b100000, width: 6, offst: 26 },
    operands: {
      rd:   { width: 5, offst: 21, type: :register },
      rs:   { width: 5, offst: 16, type: :register },
      imm5: { width: 5, offst: 11, type: :numeric }
    }
  },

  beq: {
    opcode: { val: 0b010010, width: 6, offst: 26 },
    operands: {
      rs:     { width: 5,  offst: 21, type: :register },
      rt:     { width: 5,  offst: 16, type: :register },
      offset: { width: 16, offst: 0,  type: :numeric }
    }
  },

  ld: {
    opcode: { val: 0b000010, width: 6, offst: 26 },
    operands: {
      rt:   { width: 5,  offst: 16, type: :register },
      base: { width: 5,  offst: 21, type: :register },
      imm:  { width: 14, offst: 0,  type: :numeric }
    }
  },

  cbit: {
    opcode: { val: 0b011111, width: 6, offst: 26 },
    operands: {
      rd:   { width: 5, offst: 21, type: :register },
      rs:   { width: 5, offst: 16, type: :register },
      imm5: { width: 5, offst: 11, type: :numeric }
    }
  },

  j: {
    opcode: { val: 0b110010, width: 6, offst: 26 },
    operands: {
      instr_index: { width: 26, offst: 0, type: :numeric }
    }
  },

  addi: {
    opcode: { val: 0b111011, width: 6, offst: 26 },
    operands: {
      rt:  { width: 5,  offst: 16, type: :register },
      rs:  { width: 5,  offst: 21, type: :register },
      imm: { width: 16, offst: 0,  type: :numeric }
    }
  },

  jalr: {
    opcode: { val: 0b101001, width: 6, offst: 26 },
    operands: {
      rt:  { width: 5,  offst: 16, type: :register },
      rs:  { width: 5,  offst: 21, type: :register },
      imm: { width: 16, offst: 0,  type: :numeric }
    }
  },

  st: {
    opcode: { val: 0b101100, width: 6, offst: 26 },
    operands: {
      rt:   { width: 5,  offst: 16, type: :register },
      base: { width: 5,  offst: 21, type: :register },
      imm:  { width: 14, offst: 0,  type: :numeric }
    }
  },

  stp: {
    opcode: { val: 0b111000, width: 6, offst: 26 },
    operands: {
      rt1:    { width: 5,  offst: 16, type: :register },
      rt2:    { width: 5,  offst: 11, type: :register },
      base:   { width: 5,  offst: 21, type: :register },
      offset: { width: 11, offst: 0,  type: :numeric }
    }
  },

  li: {
    opcode: { val: 0b011001, width: 6, offst: 26 },
    operands: {
      rt:  { width: 5,  offst: 16, type: :register },
      imm: { width: 16, offst: 0,  type: :numeric }
    }
  }
}

REGISTER_CNT = 32

class Assembler

  attr_reader :buf

  def initialize
    @buf = []
    @regs = {}

    REGISTER_CNT.times do |i|
      @regs["x#{i}"] = i
    end
  end

  INSTR_ENCS.each do |mnemonic, fields|
    define_method(mnemonic) { |*args|
      operands = fields[:operands].values

      if operands.size != args.size
        raise "#{mnemonic}: arg cnt mismatch"
      end

      puts "#{mnemonic} #{args.join(", ")}"

      cmd_bin = enc_field(
        fields[:opcode][:val],
        fields[:opcode][:width],
        fields[:opcode][:offst]
      )

      operands.zip(args).each do |op, val|
        # FIXME: check for arg type
        if op[:type] == :register
          cmd_bin |= enc_field(
            @regs[val],
            op[:width],
            op[:offst]
          )
        elsif op[:type] == :numeric
          cmd_bin |= enc_field(
            val.to_i(),
            op[:width],
            op[:offst]
          )
        else
          raise "unknown operand type: #{op[:type]}"
        end
      end

      buf << cmd_bin
    }
  end

  def method_missing(name, *args)
    raise "unknown instruction: #{name}"
  end
  
  REGISTER_CNT.times do |i|
    define_method("x#{i}") { i }
  end

  def write_bin(path)
    File.open(path, "wb") do |f|
      @buf.each do |cmd|
        f.write([cmd].pack("V"))
      end
    end
  end

private
  def enc_field(val, len, offst)
    (val & ((1 << len) - 1)) << offst
  end

end

as = Assembler.new

File.foreach(ARGV[0]) do |line|
  splitted = line.split(" ", 2)
  mnemonic, args = splitted[0].strip(), splitted[1].split(",")

  args.map! { |arg| arg.strip() }

  if mnemonic == "stp"
    offst_base = args.delete_at(2).delete_suffix(')')
    offst, base = offst_base.split('(')
    args << base << offst
  elsif mnemonic == "st" || mnemonic == "ld"
    args[1].delete_prefix!("[")
    args[2].delete_suffix!("]")
  end

  as.send(mnemonic, *args)

end

as.write_bin(ARGV[1])
