-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2022 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Simona Jánošíková <xjanos19 AT stud.fit.vutbr.cz>
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
  -- čítač určenia začiatku/konca príkazu pre while a do while
  signal CNT_inc: std_logic;
  signal CNT_dec: std_logic;
  signal CNT_out: std_logic_vector(7 downto 0);
  
  -- programový čítač
  signal PC_inc: std_logic;
  signal PC_dec: std_logic;
  signal PC_out: std_logic_vector(12 downto 0);

  -- čítač pamäte dát
  signal PTR_inc: std_logic;
  signal PTR_dec: std_logic;
  signal PTR_out: std_logic_vector(12 downto 0);

  -- selektor MX1 
  signal SEL_mx1: std_logic;

  -- selektor MX2
  signal SEL_mx2: std_logic_vector(1 downto 0);

  type FSMstate is (
    s_start,              -- štart procesov
    s_start_init,         -- inicializácia potrebných premenných
    s_load_instruction,   -- načítanie inštrukcií
    s_i_plus_pointer,     -- 0x3E >
    s_i_minus_pointer,    -- 0x3C < 
    s_i_plus_cell,        -- 0x2B + 
    s_i_plus_cell_help,
    s_i_minus_cell,       -- 0x2D -
    s_i_minus_cell_help,
    s_i_while_start,      -- 0x5B [
    s_i_while_start_1,  
    s_i_while_start_2,
    s_i_while_start_3,
    s_i_while_end,        -- 0x5D ]
    s_i_while_end_1,
    s_i_while_end_2,
    s_i_while_end_3,
    s_i_do_while_start,   -- 0x28 (
    s_i_do_while_start_1,
    s_i_do_while_start_2,
    s_i_do_while_start_3, 
    s_i_do_while_end,     -- 0x29 )
    s_i_do_while_end_1,
    s_i_do_while_end_2,
    s_i_do_while_end_3,
    s_i_print,            -- 0x2E .
    s_i_print_help,
    s_i_load_save,        -- 0x2C ,
    s_i_load_save_help,
    s_i_prog_end,         -- 0x00 null
    s_i_other             -- ostatné inštrukcie, komentáre 
  );

  signal fsm_currente_state: FSMstate;
  signal fsm_next_state: FSMstate;

begin

  process_cnt: process(CLK, RESET)
		begin
			if RESET = '1' then
				CNT_out <= "00000000";
			elsif rising_edge(CLK) then
				if CNT_inc = '1' then
					CNT_out <= CNT_out + 1;
				elsif CNT_dec = '1' then
					CNT_out <= CNT_out - 1;
				end if;
			end if;
	  end process;

  process_pc: process(CLK, RESET)
    begin
      if RESET = '1' then
        PC_out <= (others => '0');
      elsif rising_edge(CLK) then
        if PC_inc = '1' then
          PC_out <= PC_out + 1;
        elsif PC_dec = '1' then
          PC_out <= PC_out - 1;
        end if;    
      end if;    
    end process;
    
  process_ptr: process(CLK, RESET)
    begin
      if RESET = '1' then
        PTR_out <= "1000000000000";
      elsif rising_edge(CLK) then
        if PTR_inc = '1' then 
            PTR_out <= PTR_out + 1;  
        elsif PTR_dec = '1' then 
            PTR_out <= PTR_out - 1;  
        end if;
      end if;          
    end process;

  process_mx1: process(SEL_mx1, PC_out, PTR_out)
    begin
      case SEL_mx1 is
        -- adresa programu alebo adresa dát
        when '1'    => DATA_ADDR <= PTR_out;
        when others => DATA_ADDR <= PC_out;
      end case;  
    end process;  

  process_mx2: process(SEL_mx2, DATA_RDATA, IN_DATA)  
    begin
      case SEL_mx2 is
        -- hodnota zapisovaná do pamäti
        when "00"   => DATA_WDATA <= IN_DATA;
        when "01"   => DATA_WDATA <= DATA_RDATA - 1;
        when "10"   => DATA_WDATA <= DATA_RDATA + 1;
        when "11"   => DATA_WDATA <= DATA_RDATA;     
        when others => DATA_WDATA <= DATA_RDATA;
      end case ;
    end process;   
    
  process_FSM_state: process(CLK, RESET)
    begin
      if RESET = '1' then
        fsm_currente_state <= s_start;
      elsif rising_edge(CLK) then
        if EN = '1' then
          fsm_currente_state <= fsm_next_state;
        end if;
      end if;    
    end process; 
    
  process_FSM: process(CLK, RESET)
    begin

      -- inicializácia
      CNT_inc <= '0';
      CNT_dec <= '0';
      PC_inc  <= '0';
      PC_dec  <= '0';
      PTR_inc <= '0';
      PTR_dec <= '0';

      -- defaultná inicializácia
      DATA_RDWR <= '0';
		  DATA_EN   <= '0';
		  IN_REQ    <= '0';
		  OUT_WE    <= '0';

      case fsm_currente_state is
        -- štart procesov
        when s_start =>
          DATA_RDWR <= '0';
          SEL_mx1   <= '0';
          SEL_mx2   <= "00";

          if RESET = '1' then
            fsm_next_state <= s_start;
          else
            fsm_next_state <= s_start_init;
          end if;

        when s_start_init =>
          SEL_mx1        <= '0';
          SEL_mx2        <= "00";
          DATA_RDWR      <= '0'; 
          DATA_EN        <= '1';
          fsm_next_state <= s_load_instruction;

        when s_load_instruction =>
          case DATA_RDATA is
            when X"3E"  => fsm_next_state <= s_i_plus_pointer;
            when X"3C"  => fsm_next_state <= s_i_minus_pointer;
            when X"2B"  => fsm_next_state <= s_i_plus_cell;
            when X"2D"  => fsm_next_state <= s_i_minus_cell;
            when X"5B"  => fsm_next_state <= s_i_while_start;
            when X"5D"  => fsm_next_state <= s_i_while_end;
            when X"28"  => fsm_next_state <= s_i_do_while_start;
            when X"29"  => fsm_next_state <= s_i_do_while_end;
            when X"2E"  => fsm_next_state <= s_i_print;
            when X"2C"  => fsm_next_state <= s_i_load_save;
            when X"00"  => fsm_next_state <= s_i_prog_end;
            when others => fsm_next_state <= s_i_other;
          end case;  

        -- 0x3E >
        when s_i_plus_pointer =>  
          PC_inc         <= '1';
          PTR_inc        <= '1';
          fsm_next_state <= s_start_init;

        -- 0x3C <  
        when s_i_minus_pointer => 
          PC_inc         <= '1';
          PTR_dec        <= '1';
          fsm_next_state <= s_start_init;

        -- 0x2B +
        when s_i_plus_cell => 
          SEL_mx1        <= '1';
          SEL_mx2        <= "10";
          DATA_RDWR      <= '0';
          DATA_EN        <= '1';          
          fsm_next_state <= s_i_plus_cell_help; 

        when s_i_plus_cell_help => 
          PC_inc         <= '1';
          SEL_mx1        <= '1';
          SEL_mx2        <= "10";
          DATA_RDWR      <= '1';
          DATA_EN        <= '1';
          fsm_next_state <= s_start_init;   

        -- 0x2D -
        when s_i_minus_cell =>
          SEL_mx1        <= '1';
          SEL_mx2        <= "01";
          DATA_RDWR      <= '0';
          DATA_EN        <= '1';          
          fsm_next_state <= s_i_minus_cell_help;
        
        when s_i_minus_cell_help =>
          PC_inc         <= '1';
          SEL_mx1        <= '1';
          SEL_mx2        <= "01";
          DATA_RDWR      <= '1';
          DATA_EN        <= '1';          
          fsm_next_state <= s_start_init;  

        -- 0x5B [
        when s_i_while_start =>
          SEL_mx1        <= '1';
          SEL_mx2        <= "11";
          DATA_RDWR      <= '0';
          DATA_EN        <= '1';
          fsm_next_state <= s_i_while_start_1;
          
        when s_i_while_start_1 => 
          if DATA_RDATA = "00000000" then
            fsm_next_state <= s_i_while_start_2;
          else
            fsm_next_state <= s_start_init;
          end if;
            PC_inc <= '1';
        
        when s_i_while_start_2 =>
          SEL_mx1        <= '0';
          SEL_mx2        <= "11";
          DATA_RDWR      <= '0';
          DATA_EN        <= '1';
          fsm_next_state <= s_i_while_start_3; 
          
        when s_i_while_start_3 => 
          if DATA_RDATA = X"5B" then
            CNT_inc        <= '1';
            PC_inc         <= '1';
            fsm_next_state <= s_i_while_start_2;
          elsif DATA_RDATA = X"5D" then
            if CNT_out = "00000000" then
              PC_inc         <= '1';
              fsm_next_state <= s_start_init;
            else
              CNT_dec        <= '1';
              PC_inc         <= '1';
              fsm_next_state <= s_i_while_start_2;
            end if;
          else
            PC_inc         <= '1';
            fsm_next_state <= s_i_while_start_2;
          end if;

        -- 0x5D ]
        when s_i_while_end =>
          SEL_mx1        <= '1';
          SEL_mx2        <= "11";
          DATA_RDWR      <= '0';
          DATA_EN        <= '1';
          fsm_next_state <= s_i_while_end_1;
          
        when s_i_while_end_1 =>
          if DATA_RDATA = "00000000" then
            PC_inc         <= '1';
            fsm_next_state <= s_start_init;
          else
            PC_dec         <= '1';
            fsm_next_state <= s_i_while_end_2;
          end if;  

        when s_i_while_end_2 =>
          SEL_mx1        <= '0';
          SEL_mx2        <= "11";
          DATA_RDWR      <= '0';
          DATA_EN        <= '1';
          fsm_next_state <= s_i_while_end_3; 
        
        when s_i_while_end_3 => 
          if DATA_RDATA = X"5D" then
            CNT_inc        <= '1';
            PC_dec         <= '1';
            fsm_next_state <= s_i_while_end_2;
          elsif DATA_RDATA = X"5B" then
            if CNT_out = "00000000" then
              PC_inc         <= '1';
              fsm_next_state <= s_start_init;
            else
              CNT_dec        <= '1';
              PC_dec         <= '1';
              fsm_next_state <= s_i_while_end_2;
            end if;
          else
            PC_dec         <= '1';
            fsm_next_state <= s_i_while_end_2;
          end if; 

        -- 0x28 (
        when s_i_do_while_start =>
          SEL_mx1        <= '1';
          SEL_mx2        <= "11";
          DATA_RDWR      <= '0';
          DATA_EN        <= '1';
          fsm_next_state <= s_i_do_while_start_1;
          
        when s_i_do_while_start_1 => 
          if DATA_RDATA = "00000000" then
            fsm_next_state <= s_i_do_while_start_2;
          else
            fsm_next_state <= s_start_init;
          end if;
            PC_inc <= '1';
        
        when s_i_do_while_start_2 =>
          SEL_mx1        <= '0';
          SEL_mx2        <= "11";
          DATA_RDWR      <= '0';
          DATA_EN        <= '1';
          fsm_next_state <= s_i_do_while_start_3; 
          
        when s_i_do_while_start_3 => 
          if DATA_RDATA = X"28" then
            CNT_inc        <= '1';
            PC_inc         <= '1';
            fsm_next_state <= s_i_do_while_start_2;
          elsif DATA_RDATA = X"29" then
            if CNT_out = "00000000" then
              PC_inc         <= '1';
              fsm_next_state <= s_start_init;
            else
              CNT_dec        <= '1';
              PC_inc         <= '1';
              fsm_next_state <= s_i_do_while_start_2;
            end if;
          else
            PC_inc         <= '1';
            fsm_next_state <= s_i_do_while_start_2;
          end if;

        -- 0x29 )
        when s_i_do_while_end =>
          SEL_mx1        <= '1';
          SEL_mx2        <= "11";
          DATA_RDWR      <= '0';
          DATA_EN        <= '1';
          fsm_next_state <= s_i_do_while_end_1;
          
        when s_i_do_while_end_1 =>
          if DATA_RDATA = "00000000" then
            PC_inc         <= '1';
            fsm_next_state <= s_start_init;
          else
            PC_dec         <= '1';
            fsm_next_state <= s_i_do_while_end_2;
          end if;  

        when s_i_do_while_end_2 =>
          SEL_mx1        <= '0';
          SEL_mx2        <= "11";
          DATA_RDWR      <= '0';
          DATA_EN        <= '1';
          fsm_next_state <= s_i_do_while_end_3; 
        
        when s_i_do_while_end_3 => 
          if DATA_RDATA = X"29" then
            CNT_inc        <= '1';
            PC_dec         <= '1';
            fsm_next_state <= s_i_do_while_end_2;
          elsif DATA_RDATA = X"28" then
            if CNT_out = "00000000" then
              PC_inc         <= '1';
              fsm_next_state <= s_start_init;
            else
              CNT_dec        <= '1';
              PC_dec         <= '1';
              fsm_next_state <= s_i_do_while_end_2;
            end if;
          else
            PC_dec         <= '1';
            fsm_next_state <= s_i_do_while_end_2;
          end if; 

        -- 0x2E .
        when s_i_print =>
          SEL_mx1        <= '1';
          SEL_mx2        <= "11";
          DATA_RDWR      <= '0';
          DATA_EN        <= '1';
          fsm_next_state <= s_i_print_help;

        when s_i_print_help =>
          if OUT_BUSY = '1' then
            fsm_next_state <= s_i_print_help;
          else
            OUT_DATA       <= DATA_RDATA;
            OUT_WE         <= '1';
            PC_inc         <= '1';
            fsm_next_state <= s_start;
          end if;  

        -- 0x2C ,
        when s_i_load_save =>
          IN_REQ         <= '1';
          fsm_next_state <= s_i_load_save_help;

        when s_i_load_save_help =>
          if IN_VLD = '0' then 
            IN_REQ         <= '1';
            fsm_next_state <= s_i_load_save_help;
          else
            PC_inc         <= '1';
            SEL_mx1        <= '1';
            SEL_mx2        <= "00";
            DATA_RDWR      <= '1';
            DATA_EN        <= '1';
            fsm_next_state <= s_start;
          end if;

        -- 0x00 null
        when s_i_prog_end =>
          fsm_next_state <= s_i_prog_end;

        -- ostatné inštrukcie, komentáre
        when s_i_other =>
          PC_inc         <= '1';
          fsm_next_state <= s_start_init; 

        when others => null;
      end case;   
    end process;  

end behavioral;