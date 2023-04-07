<?php

/**
 * prvá časť projektu do predmetu IPP
 * autor: Simona Jánošíková (xjanos19)
 * parse.php vykoná lexikálnu a syntaktickú analýzu vsupného jazyka a vygeneruje XML výstup 
 */

ini_set('display_errors', 'stderr');

/**
 * Summary of ErrorPrint
 * Funkcia slúžiaca na výpis chybových hlášok a prípadné ukončenie programu v prípade chyby
 * @param int $error_code premenná obsahujúca návratovú hodnotu s ktorou sa má program ukončiť v prípade chyby
 * @return void
 */
function ErrorPrint(int $error_code) {
    $error_message = "";
    switch ($error_code) {
        case 10:
            $error_message = "Zlá kombinácia argumentov";
            break;
        case 21:
            $error_message = "Chybná hlavička súboru";
            break;
        case 22:
            $error_message = "Chybný operačný kód, prípadne dvojitá hlavička";
            break;
        case 23:
            $error_message = "Lexikálna alebo syntaktická chyba";
            break;
    }
    fwrite(STDERR, $error_message);
    exit($error_code);
}
/**
 * Summary of LoadArgs
 * Funkcia kontrolujúca správny počet argumentov
 * @param int $argc počet vstupných argumentov
 * @param array $argv pole s obsahom argumentov
 * @return void
 */
function LoadArgs(int $argc, array $argv) {
    if ($argc > 2) {
        ErrorPrint(10);
    }
    else if ($argc == 2) {
        if ($argv[1] = "--help") {
            fwrite(STDOUT, "Účel skriptu:\n");
            fwrite(STDOUT, "Skript načíta zo štandardného vstupu zdrojový kód v jazyku IPPcode23, skontroluje lexikálnu a syntaktickú korektnosť kódu a vypíše na štandardný výstup XML reprezentáciu programu\n");
            fwrite(STDOUT, "------------------------------------\n");
            fwrite(STDOUT, "Použitie:\n");
            fwrite(STDOUT, "      php8.1 parse.php [--help]\n\n");
            fwrite(STDOUT, "-------------------------------------\n");
            exit(0);
        }
        else {
            ErrorPrint(10);
        }
    }
}

/**
 * Summary of printArg
 * Funkcia kontrolujúca správnosť výrazov, vykonáva syntaktickú analýzu a generuje XML výstup
 * @param mixed $xml XML výstup
 * @param int $arg_numb poradie operandu funkcie vstupného jazyka
 * @param string $type typ danéhého operandu
 * @param string $arg operand
 * @return void
 */
function printArg($xml, int $arg_numb, string $type, string $arg) {
    if ($type === "var") {
        if (preg_match('/^(GF|TF|LF)@[a-zA-Z_\-\$&%\*\!\?ěščřžýáíéóúůďťňĎŇŤŠČŘŽÝÁÍÉÚŮĚÓ][a-zA-Z0-9_\-\$&%\*\!\?ěščřžýáíéóúůďťňĎŇŤŠČŘŽÝÁÍÉÚŮĚÓ]*$/', $arg)) {
            $xml->startElement("arg" . $arg_numb);
            $xml->writeAttribute('type', 'var');
            $xml->text($arg);
            $xml->endElement();
        }
        else {
            ErrorPrint(23);
        }
    }
    elseif ($type === "symb") {
        if (preg_match('/^(GF|TF|LF)@[a-zA-Z_\-\$&%\*\!\?ěščřžýáíéóúůďťňĎŇŤŠČŘŽÝÁÍÉÚŮĚÓ][a-zA-Z0-9_\-\$&%\*\!\?ěščřžýáíéóúůďťňĎŇŤŠČŘŽÝÁÍÉÚŮĚÓ]*$/', $arg)) {
            $xml->startElement("arg" . $arg_numb);
            $xml->writeAttribute('type', 'var');
            $xml->text($arg);
            $xml->endElement();
        }
        elseif (preg_match('/^string@([^\s#\\\\]|\\\\\d{3})*$/', $arg) || preg_match('/^nil@nil$/', $arg) || preg_match('/^bool@(true|false)$/', $arg) ||
            preg_match('/^int@(\+|-){0,1}(([1-9][0-9]*(_[0-9]+)*)|0)$/', $arg) || preg_match('/^int@(\+|-){0,1}0[xX][0-9a-fA-F]+(_[0-9a-fA-F]+)*$/', $arg) ||
            preg_match('/^int@(\+|-){0,1}0[oO]?[0-7]+(_[0-7]+)*+$/', $arg)) {
            $split = preg_split('/@/', $arg, 2, PREG_SPLIT_NO_EMPTY);
            $xml->startElement("arg" . $arg_numb);
            $xml->writeAttribute('type', $split[0]);
            $xml->text($split[1]);
            $xml->endElement();
        }
        else {
            ErrorPrint(23);
        }
    }
    elseif ($type === "label") {
        if (preg_match('/^[a-zA-Z_\-\$&%\*\!\?ěščřžýáíéóúůďťňĎŇŤŠČŘŽÝÁÍÉÚŮĚÓ][a-zA-Z0-9_\-\$&%\*\!\?ěščřžýáíéóúůďťňĎŇŤŠČŘŽÝÁÍÉÚŮĚÓ]*$/', $arg)) {
            $xml->startElement("arg" . $arg_numb);
            $xml->writeAttribute('type', 'label');
            $xml->text($arg);
            $xml->endElement();
        }
        else {
            ErrorPrint(23);
        }
    }
    else {
        if (preg_match('/^(bool|string|nil|int|)$/', $arg)) {
            $xml->startElement("arg" . $arg_numb);
            $xml->writeAttribute('type', 'type');
            $xml->text($arg);
            $xml->endElement();
        }
        else {
            ErrorPrint(23);
        }
    }
}
/**
 * Summary of arg_number
 * Funkcia kontrolujucúca či daná funkcia vstupného jazyka má korektný počet argumentov
 * @param array $args pole argumentov operácie vstupného jazyka 
 * @param int $op_args korektný počet argumentov
 * @return void
 */
function arg_number(array $args, int $op_args) {
    if (count($args) != $op_args + 1) {
        ErrorPrint(23);
    }
}

/*-------------------------- HLAVNÉ TELO PROGRAMU --------------------------*/

// začatie výstupu XML 
$xml = xmlwriter_open_memory();
xmlwriter_start_document($xml, '1.0', 'UTF-8');
xmlwriter_start_element($xml, 'program');
xmlwriter_start_attribute($xml, 'language');
xmlwriter_text($xml, 'IPPcode23');
xmlwriter_end_attribute($xml);
$line_num = 1;

// načítanie a overenie argumentov
LoadArgs($argc, $argv);
$header = false;

while ($line = fgets(STDIN)) {
    // odstránenie komentára z riadka
    $line = preg_replace('/#.*/', ' ', $line);

    if (preg_match('/(^\s+$)/i', $line)) {
        continue;
    }

    // rozparsovanie riadku do poľa pre lepšie spracovanie
    $parsed_line = preg_split('/[\s]+/', $line, -1, PREG_SPLIT_NO_EMPTY);
    
    // kontrola hlavičky vstupného jazyka
    if (!$header) {
        if (strtoupper($parsed_line[0]) === ".IPPCODE23") {
            $header = true;
            continue;
        }
        else {
            if (!$header) {
                ErrorPrint(21);
            }
        }
    }

    xmlwriter_start_element($xml, 'instruction');
    xmlwriter_start_attribute($xml, 'order');
    xmlwriter_text($xml, $line_num++);
    xmlwriter_end_attribute($xml);
    xmlwriter_start_attribute($xml, "opcode");
    xmlwriter_text($xml, strtoupper($parsed_line[0]));

    // hlavný switch inštrukcii vstupného jazyka
    // inštrukcie sú rozdelené do skupiniek podľa počtu alebo typu argumentov danej funkcie
    switch (strtoupper($parsed_line[0])) {
        case "BREAK":
        case "RETURN":
        case "CREATEFRAME":
        case "PUSHFRAME":
        case "POPFRAME":
            arg_number($parsed_line, 0);
            break;

        case "POPS":
        case "DEFVAR":
            arg_number($parsed_line, 1);
            printArg($xml, 1, "var", $parsed_line[1]);
            break;

        case "WRITE":
        case "EXIT":
        case "DPRINT":
        case "PUSHS":
            arg_number($parsed_line, 1);
            printArg($xml, 1, "symb", $parsed_line[1]);
            break;

        case "LABEL":
        case "JUMP":
        case "CALL":
            arg_number($parsed_line, 1);
            printArg($xml, 1, "label", $parsed_line[1]);
            break;

        case "READ":
            arg_number($parsed_line, 2);
            printArg($xml, 1, "var", $parsed_line[1]);
            printArg($xml, 2, "type", $parsed_line[2]);
            break;

        case "STRLEN":
        case "TYPE":
        case "MOVE":
        case "INT2CHAR":
        case "NOT":
            arg_number($parsed_line, 2);
            printArg($xml, 1, "var", $parsed_line[1]);
            printArg($xml, 2, "symb", $parsed_line[2]);
            break;

        case "ADD":
        case "SUB":
        case "MUL":
        case "IDIV":
        case "LT":
        case "GT":
        case "EQ":
        case "AND":
        case "OR":
        case "STRI2INT":
        case "CONCAT":
        case "GETCHAR":
        case "SETCHAR":
            arg_number($parsed_line, 3);
            printArg($xml, 1, "var", $parsed_line[1]);
            printArg($xml, 2, "symb", $parsed_line[2]);
            printArg($xml, 3, "symb", $parsed_line[3]);
            break;

        case "JUMPIFEQ":
        case "JUMPIFNEQ":
            arg_number($parsed_line, 3);
            printArg($xml, 1, "label", $parsed_line[1]);
            printArg($xml, 2, "symb", $parsed_line[2]);
            printArg($xml, 3, "symb", $parsed_line[3]);
            break;

        default:
            exit(22);
    }
    xmlwriter_end_element($xml);
}

//ukončenie XML výstupu
xmlwriter_end_document($xml);
echo xmlwriter_output_memory($xml);

?>