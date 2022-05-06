-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2021 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): xsvita08
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
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- ram[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_WREN  : out std_logic;                    -- cteni z pameti (DATA_WREN='0') / zapis do pameti (DATA_WREN='1')
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA obsahuje stisknuty znak klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna pokud IN_VLD='1'
   IN_REQ    : out std_logic;                     -- pozadavek na vstup dat z klavesnice
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- pokud OUT_BUSY='1', LCD je zaneprazdnen, nelze zapisovat,  OUT_WREN musi byt '0'
   OUT_WREN : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WREN='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
--------------------------------------------------------
--LOGIC VECTORS
--------------------------------------------------------
signal pc_reg   : std_logic_vector(11 downto 0);
signal ptr_reg  : std_logic_vector(9 downto 0);
signal cnt_reg   : std_logic_vector(9 downto 0);
--------------------------------------------------------
--SELECTOR FOR MUX
--------------------------------------------------------
signal selector : std_logic_vector(1 downto 0);
--------------------------------------------------------
--REGISTER INCREMENTS AND DECREMENTS
--------------------------------------------------------
signal pc_inc   : std_logic;
signal pc_dec   : std_logic;

signal ptr_inc  : std_logic;
signal ptr_dec  : std_logic;

signal cnt_inc  : std_logic;
signal cnt_dec  : std_logic;

signal pc_reset  : std_logic;
signal ptr_reset  : std_logic;
signal cnt_reset0  : std_logic;
signal cnt_reset  : std_logic;
--------------------------------------------------------
--FSM ALL INNER STATES
--------------------------------------------------------
type fsm_state is   (s_idle, s_fetch, s_decode, s_halt,
							s_writeEN,s_write, s_get_char,s_get_charEN,
							s_address_right,s_address_left,
							s_data_inc,s_di_write,s_data_dec,s_dd_write,s_di_write2,s_dd_write2,
							s_end_loops,s_end_loops2,s_end_loops3,s_end_loops4,s_loops_right,
							s_begin_loops,s_begin_loops2,s_begin_loops3,s_loops_left,
							s_breaks,s_breaks2,s_breaks3,
							s_other);
							
----states for loops(without nested loops)
----s_begin_loop,s_begin_loop2,s_loop_left,s_end_loop,s_end_loop2,s_loop_right,s_break,s_break2,
--------------------------------------------------------
--SIGNALS FOR FSM (PRESENT STATE AND NEXT STATE)
--------------------------------------------------------							
signal p_state    : fsm_state;
signal n_state    : fsm_state;--------------------------------------------------------

begin




--------------------------------------------------------
--STATE UPDATER
--------------------------------------------------------
	fsm_pstate: process (RESET, CLK)
	begin

			if (RESET='1') then
				p_state <= s_idle;
			elsif rising_edge(CLK) then
					if (EN = '1') then
						p_state <= n_state;
					end if;
			end if;
		
	end process;
--------------------------------------------------------
--------------------------------------------------------




--------------------------------------------------------
--FINITE STATE MACHINE
--------------------------------------------------------
	fsm_machine: process (IN_VLD, OUT_BUSY, CODE_DATA, EN, DATA_RDATA, p_state, cnt_reg)
	begin
--------------------------------------------------------	
--INITIALIZING SIGNALS
--------------------------------------------------------
		pc_inc    <= '0';
		pc_dec    <= '0';
		
		ptr_inc   <= '0'; 
		ptr_dec   <= '0';
		
		cnt_inc   <= '0';
		cnt_dec   <= '0';
		
		cnt_reset <= '0';
		cnt_reset0 <= '0';
		ptr_reset <= '0';
		pc_reset <= '0';
		
		--selector <= "00"; should have been functional but it is not functional with this 
		
		CODE_EN <= '0';
		DATA_EN <= '0';
		DATA_WREN <= '0';
		OUT_WREN <= '0';
		IN_REQ <= '0';	
--------------------------------------------------------
--END OF INITIALIZING SIGNALS
--------------------------------------------------------	



		case p_state is
		
		
		
--------------------------------------------------------			
--CHANGING IDLE STATE TO "START STATE"
--------------------------------------------------------	
			when s_idle => n_state <= s_fetch;			
								pc_reset <= '1';
								ptr_reset <= '1';
								cnt_reset0 <= '1';
			
--------------------------------------------------------	
--ENABLING SIGNALS FOR DECODER
--------------------------------------------------------	
			when s_fetch => n_state <= s_decode;
								 CODE_EN <= '1'; 			--reading data from ROM
								 selector <= "00";
								 
								 
								 
--------------------------------------------------------								 
--WAITING FOR STATE FROM DECODER 
--------------------------------------------------------	
			when s_decode => case CODE_DATA is		--decoding the command
										when   X"3E" => n_state <= s_address_right; 	--    >     -- 
										when   X"3C" => n_state <= s_address_left;  	--    <     --
										when   X"2B" => n_state <= s_data_inc;      	--    +     --
										when   X"2D" => n_state <= s_data_dec;      	--    -     --
										when   X"5B" => n_state <= s_begin_loops;    --    [     --
										when   X"5D" => n_state <= s_end_loops;      --    ]     --
										when   X"2E" => n_state <= s_writeEN;       	--    .     --
										when   X"2C" => n_state <= s_get_char;      	--    ,     --
										when   X"7E" => n_state <= s_breaks;         --    ~     --
										when   X"00" => n_state <= s_halt;        	--   null   --
										when others  => n_state <= s_other;		     	--  other   --
									end case;



--------------------------------------------------------									  									
--INCREMENTING PC AND PTR REGISTER
-----------------[    >     ]---------------------------
			when s_address_right => n_state <= s_fetch;
											ptr_inc <= '1';
											pc_inc <= '1';



--------------------------------------------------------																				  
--DECREMENTING PC AND PTR REGISTER
-----------------[    <     ]---------------------------
			when s_address_left =>  n_state <= s_fetch;
											ptr_dec <= '1';
											pc_inc <= '1';



--------------------------------------------------------												
--REWRITING VALUE ON ADDRESS R_DATA+1 
-----------------[    +     ]---------------------------
			when s_data_inc =>  	n_state <= s_di_write;
										DATA_EN <= '1';
										DATA_WREN <= '0';
										
			when s_di_write =>   selector <= "10";
										n_state <= s_di_write2;
										
			when s_di_write2 =>	n_state <= s_fetch;
										DATA_EN <= '1';
										DATA_WREN <= '1';
										pc_inc <= '1';

			
			
--------------------------------------------------------				
--REWRITING VALUE ON ADDRESS R_DATA-1 											
-----------------[    -     ]---------------------------
			when s_data_dec => n_state <= s_dd_write;
									 DATA_EN <= '1';
									 DATA_WREN <= '0';
			
			when s_dd_write =>   selector <= "01";
										n_state <= s_dd_write2;
										

			when s_dd_write2 =>	n_state <= s_fetch;
										DATA_EN <= '1';
										DATA_WREN <= '1';
										pc_inc <= '1';
										
										
										
--------------------------------------------------------	
--SIMPLE LOOPS (WITHOUT NESTED LOOPS)
-----------------[    [     ]---------------------------
--			when s_begin_loop => n_state <= s_begin_loop2;
--										pc_inc <= '1';
--										DATA_WREN <= '0';
--										DATA_EN <= '1';
			
--			when s_begin_loop2 => if (DATA_RDATA = "00000000") then
--											n_state <= s_loop_left;
--											CODE_EN <= '1';
--										else
--											n_state <= s_fetch;
--										end if;
										
--			when s_loop_left => if (CODE_DATA = X"5D") then
--											n_state <= s_fetch;
--									  else
--											n_state <= s_loop_left;
--											CODE_EN <= '1';
--											pc_inc <= '1';
--									  end if;
									  
-----------------[    ]     ]---------------------------									
--			when s_end_loop => n_state <= s_end_loop2;
--									 DATA_WREN <= '0';
--									 DATA_EN <= '1';
			
--			when s_end_loop2 => 	if (DATA_RDATA /= "00000000") then
--											n_state <= s_loop_right;
--											CODE_EN <= '1';
--										else
--											n_state <= s_fetch;
--											pc_inc <= '1';
--										end if;
										
--			when s_loop_right => if (CODE_DATA = X"5B") then
--											n_state <= s_fetch;
--											pc_inc <= '1';
--									  else
--											n_state <= s_loop_right;
--											pc_dec <= '1';
--											CODE_EN <= '1';
--									  end if;
									  
-----------------[    ~     ]---------------------------		
--			when s_break =>   n_state <= s_break2;
--									pc_inc <= '1';
--									CODE_EN <= '1';
									
--			when s_break2 =>	if (CODE_DATA = X"5D") then
--											n_state <= s_fetch;
--									else
--											n_state <= s_break2;
--											pc_inc <= '1';
--											CODE_EN <= '1';
--									end if;
--------------------------------------------------------	
--END OF SIMPLE LOOPS (WITHOUT NESTED LOOPS)
--------------------------------------------------------



--------------------------------------------------------	
--LOOPS AND BREAK OPERATION
-----------------[    [     ]---------------------------	
when s_begin_loops => n_state <= s_begin_loops2;
							 pc_inc <= '1';
							 DATA_WREN <= '0';
							 DATA_EN <= '1';
							 
when s_begin_loops2 => if (DATA_RDATA = "00000000") then
									n_state <= s_begin_loops3;
									CODE_EN <= '1';
									cnt_reset <= '1';
								else
									n_state <= s_fetch;
								end if;
								
when s_begin_loops3 => if (cnt_reg = "0000000000") then
									n_state <= s_fetch;
								else
									n_state <= s_loops_left;
									
									if (CODE_DATA = X"5D") then
										cnt_dec <= '1';
									elsif (CODE_DATA = X"5B") then
										cnt_inc <= '1';
									end if;
									
									pc_inc <= '1';
								end if;
								
when s_loops_left =>  n_state <= s_begin_loops3;
						  CODE_EN <= '1';

-----------------[    ]     ]---------------------------	

when s_end_loops => n_state <= s_end_loops2;
							 DATA_WREN <= '0';
							 DATA_EN <= '1';
							 
when s_end_loops2 => if (DATA_RDATA /= "00000000") then
									n_state <= s_loops_right;
									--CODE_EN <= '1';
									cnt_reset <= '1';
									pc_dec <= '1';
								else
									n_state <= s_fetch;
									pc_inc <= '1';
								end if;
								
when s_end_loops3 => if (cnt_reg = "0000000000") then
									n_state <= s_fetch;
								else
									n_state <= s_end_loops4;
									
									if (CODE_DATA = X"5D") then
										cnt_inc <= '1';
									elsif (CODE_DATA = X"5B") then
										cnt_dec <= '1';
									end if;
									
								end if;
								
when s_end_loops4 => if (cnt_reg = "0000000000") then
									pc_inc <= '1';
							 else
									pc_dec <= '1';
							 end if;
							 
							 n_state <= s_loops_right;
							 
when s_loops_right => n_state <= s_end_loops3;
						    CODE_EN <= '1';

-----------------[    ~     ]---------------------------	

when s_breaks => n_state <= s_breaks3;
						pc_inc <= '1';
						cnt_reset <= '1';
						CODE_EN <= '1';
						
when s_breaks2 => if (cnt_reg = "0000000000") then
								n_state <= s_fetch;
						else
								n_state <= s_breaks3;
									
							if (CODE_DATA = X"5D") then
									cnt_dec <= '1';
							elsif (CODE_DATA = X"5B") then
									cnt_inc <= '1';
							end if;
														
							pc_inc <= '1';
							
						end if;
						
when s_breaks3 => n_state <= s_breaks2;
						CODE_EN <= '1';

--------------------------------------------------------
--END OF LOOPS AND BREAK OPERATION
--------------------------------------------------------


		
--------------------------------------------------------	
--GETTING USER INPUT
-----------------[    ,     ]---------------------------
			when s_get_charEN => n_state <= s_get_char;
										selector <= "00";
										IN_REQ <= '1';
									 
			when s_get_char => if (IN_VLD = '1') then
										n_state <= s_fetch;
										DATA_EN <= '1';
										DATA_WREN <= '1';
										pc_inc <= '1';
									 else
										n_state <= s_get_char;
										selector <= "00";
										IN_REQ <= '1';
									 end if;
									 
									 								 
							
--------------------------------------------------------				
--WRITE DATA ON DISPLAY			
-----------------[    .     ]---------------------------	
			when s_writeEN => n_state <= s_write;
									DATA_EN <=  '1';
									OUT_WREN <= '0';
								 

			when s_write =>   if (OUT_BUSY = '0') then
										n_state <= s_fetch;
										OUT_WREN <= '1';
										pc_inc <= '1';	
									else 
										n_state <= s_write;
										DATA_EN <=  '1';
										OUT_WREN <= '0';
									end if;
									
	
	
--------------------------------------------------------																						 
--NULL STATE (PC IS IN THE SAME STATE, GOING TO "START STATE") 
-----------------[    null     ]------------------------
			when s_halt =>  n_state <= s_halt;
								 pc_inc <= '0';
									
									
																		
--------------------------------------------------------	
--OTHER STATES			
-----------------[    others     ]----------------------		
			when s_other => n_state <= s_fetch;
								pc_inc <= '1';


			
--------------------------------------------------------	
--NOT DEFINED STATES			
--------------------------------------------------------				
			when others => null;
			
			
			
		end case;	




	end process;
	
--------------------------------------------------------
--END OF FINITE STATE MACHINE	
--------------------------------------------------------
	
--ASSIGN DATA FROM RAM TO OUTPUT
--------------------------------------------------------	
	OUT_DATA <= DATA_RDATA;
	
	
	





--------------------------------------------------------
--PROGRAM COUNTER
--------------------------------------------------------
	pc_register: process (RESET, CLK)
	begin

		if (RESET='1') then
			pc_reg <= (others=>'0');
		elsif rising_edge(CLK) then
			
			if (pc_inc='1') then
				pc_reg <= pc_reg + '1';
			elsif (pc_dec='1') then
				pc_reg <= pc_reg - '1';
			elsif (pc_reset='1') then
				pc_reg <= "000000000000";
			end if;
			
		end if;
		
	end process;
--------------------------------------------------------
--END OF PROGRAM COUNTER
--------------------------------------------------------
	CODE_ADDR <= pc_reg;





--------------------------------------------------------
--DATA POINTER
--------------------------------------------------------
	ptr_register: process (RESET, CLK)
	begin

		if (RESET='1') then
			ptr_reg <= (others=>'0');
		elsif rising_edge(CLK) then
			
			if (ptr_inc='1') then
				ptr_reg <= ptr_reg + '1';
			elsif (ptr_dec='1') then
				ptr_reg <= ptr_reg - '1';
			elsif (ptr_reset='1') then
				ptr_reg <= "0000000000";
			end if;
			
		end if;
		
		
	end process;--------------------------------------------------------
--END OF DATA POINTER
--------------------------------------------------------
	DATA_ADDR <= ptr_reg;





--------------------------------------------------------
--COUNTER
--------------------------------------------------------
	cnt_register: process (RESET, CLK)
	begin

		if (RESET='1') then
			cnt_reg <= (others=>'0');
		elsif rising_edge(CLK) then
			if (cnt_inc='1') then
				cnt_reg <= cnt_reg + '1';
			elsif (cnt_dec='1') then
				cnt_reg <= cnt_reg - '1';
			elsif (cnt_reset0='1') then
				cnt_reg <= "0000000000";
			elsif (cnt_reset='1') then
				cnt_reg <= "0000000001";
			end if;

		end if;

	end process;
--------------------------------------------------------
--END OF COUNTER
--------------------------------------------------------




--------------------------------------------------------
--MULTIPLEXOR
--------------------------------------------------------
with selector select 
			DATA_WDATA <=  IN_DATA when "00", 
								DATA_RDATA + 1 when  "10",
								DATA_RDATA - 1 when  "01",
								"00000000" when others;								
--------------------------------------------------------
--END OF MULTIPLEXOR
--------------------------------------------------------

 -- zde dopiste vlastni VHDL kod

 -- pri tvorbe kodu reflektujte rady ze cviceni INP, zejmena mejte na pameti, ze 
 --   - nelze z vice procesu ovladat stejny signal,
 --   - je vhodne mit jeden proces pro popis jedne hardwarove komponenty, protoze pak
 --      - u synchronnich komponent obsahuje sensitivity list pouze CLK a RESET a 
 --      - u kombinacnich komponent obsahuje sensitivity list vsechny ctene signaly.

end behavioral;
--------------------------------------------------------
-----------------END OF CPU COMPONENT-------------------