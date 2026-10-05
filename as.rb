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
    @labels = {}

    REGISTER_CNT.times do |i|
      @regs["x#{i}"] = i
    end
  end

  def collect_labels(filename)
    pc = 0
    File.foreach(filename) do |line|
      line.strip!()
      next if line.empty?()
      ind = line.index(':')
      if ind != nil
        if !@labels.has_key?(line[0,ind])
          @labels[line[0,ind]] = pc
        else
          raise "duplicate label"
        end
      else 
        pc += 1
      end
    end
  end

  def assemble(filename)
    pc = 0
    File.foreach(filename) do |line|

      ind = line.index(':')
      next if line.index(':') != nil
      next if line.strip().empty?()

      splitted = line.split(" ", 2)
      mnemonic, args = splitted[0].strip(), splitted[1].split(",")
      args.map!(&:strip)

      case mnemonic
      when "stp"
        offst_base = args.delete_at(2).delete_suffix(')')
        offst, base = offst_base.split('(')
        args << base << offst
      when "st", "ld"
        args[1].delete_prefix!("[")
        args[2].delete_suffix!("]")
      when "beq"
        args[2] = label_to_pc(args[2]) - pc
      when "j"
        args[0] = label_to_pc(args[0])
      end

      self.send(mnemonic, *args)
      pc += 1
    end
  end

private

  INSTR_ENCS.each do |mnemonic, fields|
    define_method(mnemonic) { |*args|
      operands = fields[:operands].values

      if operands.size != args.size
        raise "#{mnemonic}: arg cnt mismatch"
      end

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
            Integer(val, 10)
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

  REGISTER_CNT.times do |i|
    define_method("x#{i}") { i }
  end

  def method_missing(name, *args)
    raise "unknown instruction: #{name}"
  end

  def label_to_pc(lbl)
    if @labels.has_key?(lbl)
      return @labels[lbl]
    else
      raise "unknown label #{lbl}"
    end
  end

public
  def write_bin(path)
    File.open(path, "wb") do |f|
      @buf.each { |cmd| f.write([cmd].pack("V")) }
    end
  end

private
  def enc_field(val, len, offst)
    (val & ((1 << len) - 1)) << offst
  end

end

as = Assembler.new
as.collect_labels(ARGV[0])
as.assemble(ARGV[0])
as.write_bin(ARGV[1])
