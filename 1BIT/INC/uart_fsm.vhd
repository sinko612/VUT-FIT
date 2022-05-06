-- uart_fsm.vhd: UART controller - finite state machine
-- Author(s): Simona Jáno?íková xjanos19
--
library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------
entity UART_FSM is
port(
   CLK      : in std_logic;
   RST      : in std_logic;
   DIN      : in std_logic;
   CNT_DATA : in std_logic_vector(3 downto 0);
   CNT_READ : in std_logic_vector(4 downto 0);
   ST_WAIT  : out std_logic;
   ST_DATA  : out std_logic;
   VLD_OUT  : out std_logic
   );
end entity UART_FSM;

-------------------------------------------------
architecture behavioral of UART_FSM is
  type possible_condition is (START, WAIT_C, READ, STOP);
  signal condition : possible_condition := START;   
  
begin
   process (CLK)
   begin
      if (CLK'event and CLK='1') then 
        ST_WAIT <= '0';  
        ST_DATA <= '0';
        VLD_OUT <= '0';
        
        if (RST) = '1' then
          ST_WAIT <= '0';  
          ST_DATA <= '0';
          VLD_OUT <= '0'; 
          condition <= START;
          
        elsif (condition) = START and DIN = '1' then
          condition <= START;
          ST_WAIT <= '0';
          ST_DATA <= '0';
          VLD_OUT <= '0';
          
        elsif (condition) = START and DIN = '0' then
          condition <= WAIT_C;
          ST_WAIT <= '1'; 
          ST_DATA <= '0';
          
        elsif (condition) = WAIT_C and (CNT_READ) =  "01000" then  
          condition <= READ;
          ST_WAIT <= '1';
          ST_DATA <= '1';
          
        elsif (condition) = WAIT_C and (CNT_READ) /= "01000" then 
          condition <= WAIT_C;
          ST_WAIT <= '1';
          ST_DATA <= '0';
          
        elsif (condition) = READ and (CNT_DATA) = "1000" and DIN = '1' then
          condition <= STOP;
          ST_WAIT <= '0';
          ST_DATA <= '0';
          
        elsif (condition) = READ and (CNT_DATA) /= "1000" then 
          condition <= READ;
          ST_WAIT <= '1';
          ST_DATA <= '1';
          
        elsif (condition) = STOP then  
          condition <= START;
          VLD_OUT <= '1';
                
        end if;  
        
      end if;
   end process;   
end behavioral;