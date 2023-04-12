import sys
import re
import xml.etree.ElementTree as ET


class Frames:
    def __init__(self):
        self.Fstack = []
        self.GFrame = {}
        self.TFrame = None

    def createFrame(self):
        self.TFrame = {}

    def popFrame(self):
        if not self.Fstack:
            print("No local frame to pop!", file=sys.stderr)
            sys.exit(55)
        else:
            self.TFrame = self.Fstack.pop()

    def pushFrame(self):
        if self.TFrame == None:
            print("No temporary frame to push!", file=sys.stderr)
            sys.exit(55)
        else:
            self.Fstack.append(self.TFrame)
            self.TFrame = None

    def find(self, findFrame, findName):
        if findFrame == "GF":
            return self.GFrame[findName]
        elif findFrame == "LF":
            if not self.Fstack:
                print("Trying to find variable in notexisting local frame!",
                      file=sys.stderr)
                sys.exit(55)
            elif findName in self.Fstack[-1]:
                return self.Fstack[-1][findName]
        elif findFrame == "TF":
            if self.TFrame == None:
                print("Trying to find variable in notexisting temporary frame!",
                      file=sys.stderr)
                sys.exit(55)
            elif findName in self.TFrame:
                return self.TFrame[findName]
        return None

    def definition(self, insertFrame, insertName):
        if self.find(insertFrame, insertName):
            print("Redefinition of variable in concrete frame!", file=sys.stderr)
            sys.exit(52)
        else:
            if insertFrame == "GF":
                self.GFrame[insertName] = ["", None]
            elif insertFrame == "LF":
                self.Fstack[-1][insertName] = ["", None]
            elif insertFrame == "TF":
                self.TFrame[insertName] = ["", None]

    def update(self, updateFrame, updateName, updateValue, updateType):
        if updateFrame == "GF":
            self.GFrame[updateName] = [updateValue, updateType]
        elif updateFrame == "LF":
            self.Fstack[-1][updateName] = [updateValue, updateType]
        elif updateFrame == "TF":
            self.TFrame[updateName] = [updateValue, updateType]


class CodeInterpret():
    def __init__(self, labelArray, FILEinput):
        self.labelArray = labelArray
        self.FILEinput = FILEinput
        self.framesStacks = Frames()
        self.dataStack = []
        self.instrOrder = 0

        if self.FILEinput != None:
            try:
                self.FILEinput = open(self.FILEinput, "r")
            except:
                print(
                    "Error while trying to open or read from input file!", file=sys.stderr)
                sys.exit(11)

    def stringConversion(self, value):
        try:
            value = re.sub('&lt;', '<', value)
            value = re.sub('&gt;', '>', value)
            value = re.sub('&amp;', '&', value)
            value = re.sub('&quot;', '"', value)
            value = re.sub('&apos;', '\'', value)

            while re.search('\\\\\d{3}', value):
                value = re.sub('\\\\\d{3}', chr(
                    int(re.search('\\\\\d{3}', value)[0][1:])), value, count=1)
        except:
            return value
        return value

    def checkIfHascorrectArgs(self, XMLinstr, a1, a2, a3):
        c1 = len(XMLinstr.findall('arg1'))
        c2 = len(XMLinstr.findall('arg2'))
        c3 = len(XMLinstr.findall('arg3'))

        if c1 != a1 or c2 != a2 or c3 != a3:
            print("Wrong arguments in instruction!", file=sys.stderr)
            sys.exit(32)

    def checkVarName(self, name):
        splitN = name.split("@")
        if splitN[0] != "GF" and splitN[0] != "LF" and splitN[0] != "TF":
            print("Wrong var name!", file=sys.stderr)
            sys.exit(53)

    def checkIfIsSymb(self, XMLtype, text):
        if XMLtype == "var":
            self.checkVarName(text)
        elif XMLtype == "int" or XMLtype == "bool" or XMLtype == "string" or XMLtype == "nil":
            pass
        else:
            print("Wrong operand types!", file=sys.stderr)
            sys.exit(53)

    def checkType(self, argXML, type):
        typeXML = argXML.attrib.get("type")
        if len(argXML.attrib) != 1:
            print("Wrong atrributes count in instruction!", file=sys.stderr)
            sys.exit(32)

        if type == "symb":
            self.checkIfIsSymb(typeXML, argXML.text)
        else:
            if typeXML != type:
                print("Wrong type in instruction!", file=sys.stderr)
                sys.exit(53)
            if type == "var":
                self.checkVarName(argXML.text)
            elif type == "label":
                pass
            elif type == "type":
                pass

    def instructionCodeInterpret(self,  instrXML, instrOpcode,  iter):
        self.instrOrder += 1

        if (instrOpcode == "MOVE"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 0)
            self.checkType(instrXML.find('arg1'), "var")
            self.checkType(instrXML.find('arg2'), "symb")

        elif (instrOpcode == "CREATEFRAME"):
            self.checkIfHascorrectArgs(instrXML, 0, 0, 0)

        elif (instrOpcode == "PUSHFRAME"):
            self.checkIfHascorrectArgs(instrXML, 0, 0, 0)

        elif (instrOpcode == "POPFRAME"):
            self.checkIfHascorrectArgs(instrXML, 0, 0, 0)

        elif (instrOpcode == "DEFVAR"):
            self.checkIfHascorrectArgs(instrXML, 1, 0, 0)
            self.checkType(instrXML.find('arg1'), "var")

        elif (instrOpcode == "CALL"):
            self.checkIfHascorrectArgs(instrXML, 1, 0, 0)
            self.checkType(instrXML.find('arg1'), "label")

        elif (instrOpcode == "RETURN"):
            self.checkIfHascorrectArgs(instrXML, 0, 0, 0)

        elif (instrOpcode == "PUSHS"):
            self.checkIfHascorrectArgs(instrXML, 1, 0, 0)
            self.checkType(instrXML.find('arg1'), "symb")

        elif (instrOpcode == "POPS"):
            self.checkIfHascorrectArgs(instrXML, 1, 0, 0)
            self.checkType(instrXML.find('arg1'), "var")

        elif (instrOpcode == "ADD"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 1)
            self.checkType(instrXML.find('arg1'), "var")
            self.checkType(instrXML.find('arg2'), "symb")
            self.checkType(instrXML.find('arg3'), "symb")

        elif (instrOpcode == "SUB"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 1)
            self.checkType(instrXML.find('arg1'), "var")
            self.checkType(instrXML.find('arg2'), "symb")
            self.checkType(instrXML.find('arg3'), "symb")

        elif (instrOpcode == "MUL"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 1)
            self.checkType(instrXML.find('arg1'), "var")
            self.checkType(instrXML.find('arg2'), "symb")
            self.checkType(instrXML.find('arg3'), "symb")

        elif (instrOpcode == "IDIV"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 1)
            self.checkType(instrXML.find('arg1'), "var")
            self.checkType(instrXML.find('arg2'), "symb")
            self.checkType(instrXML.find('arg3'), "symb")

        elif (instrOpcode == "LT"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 1)
            self.checkType(instrXML.find('arg1'), "var")
            self.checkType(instrXML.find('arg2'), "symb")
            self.checkType(instrXML.find('arg3'), "symb")

        elif (instrOpcode == "GT"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 1)
            self.checkType(instrXML.find('arg1'), "var")
            self.checkType(instrXML.find('arg2'), "symb")
            self.checkType(instrXML.find('arg3'), "symb")

        elif (instrOpcode == "EQ"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 1)
            self.checkType(instrXML.find('arg1'), "var")
            self.checkType(instrXML.find('arg2'), "symb")
            self.checkType(instrXML.find('arg3'), "symb")

        elif (instrOpcode == "AND"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 1)
            self.checkType(instrXML.find('arg1'), "var")
            self.checkType(instrXML.find('arg2'), "symb")
            self.checkType(instrXML.find('arg3'), "symb")

        elif (instrOpcode == "OR"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 1)
            self.checkType(instrXML.find('arg1'), "var")
            self.checkType(instrXML.find('arg2'), "symb")
            self.checkType(instrXML.find('arg3'), "symb")

        elif (instrOpcode == "NOT"):
            # self.checkIfHascorrectArgs(instrXML, 1, 1, 1)
            # self.checkType(instrXML.find('arg1'), "var")
            # self.checkType(instrXML.find('arg2'), "symb")
            # self.checkType(instrXML.find('arg3'), "symb")
            pass

        elif (instrOpcode == "INT2CHAR"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 0)
            self.checkType(instrXML.find('arg1'), "var")
            self.checkType(instrXML.find('arg2'), "symb")

        elif (instrOpcode == "STRI2INT"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 1)
            self.checkType(instrXML.find('arg1'), "var")
            self.checkType(instrXML.find('arg2'), "symb")
            self.checkType(instrXML.find('arg3'), "symb")

        elif (instrOpcode == "READ"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 0)
            self.checkType(instrXML.find('arg1'), "var")
            self.checkType(instrXML.find('arg2'), "type")

        elif (instrOpcode == "WRITE"):
            self.checkIfHascorrectArgs(instrXML, 1, 0, 0)
            self.checkType(instrXML.find('arg1'), "symb")

        elif (instrOpcode == "CONCAT"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 1)
            self.checkType(instrXML.find('arg1'), "var")
            self.checkType(instrXML.find('arg2'), "symb")
            self.checkType(instrXML.find('arg3'), "symb")

        elif (instrOpcode == "STRLEN"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 0)
            self.checkType(instrXML.find('arg1'), "var")
            self.checkType(instrXML.find('arg2'), "symb")

        elif (instrOpcode == "GETCHAR"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 1)
            self.checkType(instrXML.find('arg1'), "var")
            self.checkType(instrXML.find('arg2'), "symb")
            self.checkType(instrXML.find('arg3'), "symb")

        elif (instrOpcode == "SETCHAR"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 1)
            self.checkType(instrXML.find('arg1'), "var")
            self.checkType(instrXML.find('arg2'), "symb")
            self.checkType(instrXML.find('arg3'), "symb")

        elif (instrOpcode == "TYPE"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 0)
            self.checkType(instrXML.find('arg1'), "var")
            self.checkType(instrXML.find('arg2'), "symb")

        elif (instrOpcode == "LABEL"):
            pass

        elif (instrOpcode == "JUMP"):
            self.checkIfHascorrectArgs(instrXML, 1, 0, 0)
            self.checkType(instrXML.find('arg1'), "label")

        elif (instrOpcode == "JUMPIFEQ"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 1)
            self.checkType(instrXML.find('arg1'), "label")
            self.checkType(instrXML.find('arg2'), "symb")
            self.checkType(instrXML.find('arg3'), "symb")

        elif (instrOpcode == "JUMPIFNEQ"):
            self.checkIfHascorrectArgs(instrXML, 1, 1, 1)
            self.checkType(instrXML.find('arg1'), "label")
            self.checkType(instrXML.find('arg2'), "symb")
            self.checkType(instrXML.find('arg3'), "symb")

        elif (instrOpcode == "EXIT"):
            self.checkIfHascorrectArgs(instrXML, 1, 0, 0)
            self.checkType(instrXML.find('arg1'), "symb")

        elif (instrOpcode == "DPRINT"):
            self.checkIfHascorrectArgs(instrXML, 1, 0, 0)
            self.checkType(instrXML.find('arg1'), "symb")

        elif (instrOpcode == "BREAK"):
            self.checkIfHascorrectArgs(instrXML, 0, 0, 0)

        else:
            print("Wrong instruction opcode name!", file=sys.stderr)
            sys.exit(32)
        return 0


class HandleXML():
    def __init__(self, FILEsource):
        if (FILEsource != None):
            try:
                tree = ET.parse(FILEsource)
            except ET.ParseError:
                print("Wrong XML format!", file=sys.stderr)
                sys.exit(31)
            except:
                print(
                    "Error while trying to open or read from source file!", file=sys.stderr)
                sys.exit(11)
        else:
            try:
                tree = ET.parse(sys.stdin)
            except:
                print("Wrong XML format!", file=sys.stderr)
                sys.exit(31)

        try:
            self.root = tree.getroot()
        except:
            print("Wrong XML format!", file=sys.stderr)
            sys.exit(31)

    def checkCorrectHeader(self):
        if self.root.attrib.get('language') != "IPPcode23" or self.root.tag != "program":
            print("Invalid XML root attribudes!", file=sys.stderr)
            sys.exit(32)

        """ for elem in self.root:
            if elem.tag != "instruction":
                print("Invalid XML element!", file=sys.stderr)
                sys.exit(32)

        if self.root.findall('arg1') or self.root.findall('arg2') or self.root.findall('arg3'):
            print("Wrong XML!", file=sys.stderr)
            sys.exit(32) """

    def checkCorrectAttributes(self):
        for i in self.root.iter():
            if (i.tag != "program" and i.tag != "instruction" and i.tag != "arg1" and i.tag != "arg2" and i.tag != "arg3"):
                print("Wrong XML - invalit tag!", file=sys.stderr)
                sys.exit(32)

    def sortXMLinCorrcetOrder(self):
        """ Pokus o zoradenie inštrukcii do korektného poradia, v prípade chýbajúceho atribútu order alebo chýbajucého čísla poradia ukončí chybou. """
        try:
            self.root[:] = sorted(
                self.root, key=lambda child: (int(child.get("order"))))
        except:
            print("Missing or wrong order attribute in XML!", file=sys.stderr)
            sys.exit(32)

    def checkIfCorrectInstrOrder(self):
        prevPosition = 0
        for instr in self.root:
            order = int(instr.attrib.get('order'))
            if order <= 0 or order == prevPosition:
                print("Wrong instruction order number!", file=sys.stderr)
                exit(32)
            prevPosition += 1

    def checkIfInstrHasOpcode(self):
        for op in self.root:
            opcode = op.attrib.get('opcode')
            if opcode == None:
                print("Missing instruction opcode!", file=sys.stderr)
                sys.exit(32)

    def saveXMLinstructions(self):
        tmpArray = []
        for instr in self.root.iter('instruction'):
            tmpArray.append(instr)
        return tmpArray


def checkAndStartInterpret(FILEsource, FILEinput):
    readXML = HandleXML(FILEsource)
    readXML.checkCorrectHeader()
    readXML.checkCorrectAttributes()
    readXML.sortXMLinCorrcetOrder()
    readXML.checkIfCorrectInstrOrder()
    readXML.checkIfInstrHasOpcode()

    instructionArray = []
    instructionArray = readXML.saveXMLinstructions()

    labelArray = {}

    positionOrder = 0
    for i_ction in instructionArray:
        if (i_ction.attrib.get('opcode').upper() == "LABEL"):

            a1 = len(i_ction.findall('arg1'))
            a2 = len(i_ction.findall('arg2'))
            a3 = len(i_ction.findall('arg3'))

            if a1 != 1 or a2 != 0 or a3 != 0:
                print("Wrong arguments in LABEL funkction!", file=sys.stderr)
                sys.exit(32)

            arg = i_ction.find('arg1')
            if len(arg.attrib) != 1 or arg.attrib.get("type") != "label":
                print("Wrong atrributes in instruction LABEL!", file=sys.stderr)
                sys.exit(32)

            if i_ction.find('arg1').text in labelArray:
                print("Redefinition of LABEL!", file=sys.stderr)
                sys.exit(52)
            else:
                labelArray[i_ction.find('arg1').text] = positionOrder
        positionOrder += 1

    interpret = CodeInterpret(labelArray, FILEinput)
    iterCounter = 0
    while iterCounter < len(instructionArray):
        a = interpret.instructionCodeInterpret(instructionArray[iterCounter], instructionArray[iterCounter].attrib.get(
            'opcode').upper(), iterCounter)
        iterCounter += 1


def checkArguments():

    source = None
    input = None

    if len(sys.argv) == 2:
        if sys.argv[1] == "--help":
            print("")
            print("Project for IPP course in FIT VUT")
            print(
                "Program loads input in XML formath, interprets it and generates an output.")
            print("")
            print("Correct usage:")
            print("")
            print(" Show help:")
            print("     python3.10 interpret.py --help")
            print("")
            print(" Run program with sourcefile:")
            print("     python3.10 interpret.py --source=file")
            print("")
            print(" Run program with inputfile:")
            print("     python3.10 interpret.py --input=file")
            print("")
            print(" Run program with sourcefile and inputfile:")
            print("     python3.10 interpret.py  --source=file --input=file")
            sys.exit(0)
        elif re.search(r"^--source=.+$", sys.argv[1]):
            source = re.findall(r"(?<=\=).*", sys.argv[1])[0]
        elif re.search(r"^--input=.+$", sys.argv[1]):
            input = re.findall(r"(?<=\=).*", sys.argv[1])[0]
        else:
            print("Wrong argument formath!", file=sys.stderr)
            sys.exit(10)
    elif len(sys.argv) == 3:

        r = re.compile(r"^(--source=|--input=).+$")

        sourceFiles = list(filter(r.match, sys.argv))
        if len(sourceFiles) != 2:
            print("Not specified input or source file!", file=sys.stderr)
            sys.exit(10)

        for arg in sourceFiles:
            arg = re.split(r"=", arg)
            if len(arg) != 2:
                print("Not specified input or source file!asdasd", file=sys.stderr)
                sys.exit(10)  # neviem ci tu toto uplne musi byt

            if arg[0] == "--source" and source == None:
                source = arg[1]
            elif arg[0] == "--input" and input == None:
                input = arg[1]
            else:
                print("Argument entered twice!", file=sys.stderr)
                sys.exit(10)
    else:
        print("Wrong argument count!", file=sys.stderr)
        sys.exit(10)

    if input == None and source == None:
        print("At least one file must be specified!", file=sys.stderr)
        sys.exit(10)

    return source, input


if __name__ == '__main__':
    FILEsource, FILEinput = checkArguments()
    checkAndStartInterpret(FILEsource, FILEinput)
    sys.exit(0)
