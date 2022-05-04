-- uart_fsm.vhd: UART controller - finite state machine
-- Author(s): xkrali20
--
library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------
entity UART_FSM is
port(
   CLK           : in std_logic;
   RST           : in std_logic;
   DIN           : in std_logic;
   CNT_DATA      : in std_logic_vector(3 downto 0);
   TIME_CNT      : in std_logic_vector(4 downto 0);
   CNT_TIME_EN   : out std_logic;
   DOUT_VLD      : out std_logic;
   READ_DATA_EN  : out std_logic
      
   );
end entity UART_FSM;

-------------------------------------------------
architecture behavioral of UART_FSM is
  
  type t_state is (WAIT_IDLE, START, READ_DATA);
  signal state : t_state := WAIT_IDLE;
  
  
begin
  
  
  
  
  process (CLK)
  begin  
    ------ detekcia nabeznej hrany hodinoveho signalu -----
    if (CLK'event and CLK='1') then
          
      CNT_TIME_EN <= '0';
      DOUT_VLD <= '0';
      READ_DATA_EN <= '0';
      
      
      if(RST) = '1' then
          state <= WAIT_IDLE;
          READ_DATA_EN <= '0';
          CNT_TIME_EN <= '0';
          DOUT_VLD <= '0';

             
      elsif(state) = READ_DATA and CNT_DATA = "1000" and DIN = '1' then
          state <= WAIT_IDLE;
          DOUT_VLD <= '1';
          READ_DATA_EN <= '0';
          CNT_TIME_EN <= '0';
                  

      elsif (state) = READ_DATA then
          state <= READ_DATA;
          READ_DATA_EN <= '1';
          CNT_TIME_EN <= '1';
          
          
      elsif (state) = START and (TIME_CNT) = "01000" then 
          state <= READ_DATA;    
          CNT_TIME_EN <= '1';
          READ_DATA_EN <= '1';
  
          
      elsif (state) = START then
          CNT_TIME_EN <= '1';
          READ_DATA_EN <= '0';
          
            
          
      elsif (state) = WAIT_IDLE and DIN = '0' then
          state <= START;        
          READ_DATA_EN <= '0';
          CNT_TIME_EN <='1';
          
      elsif (state) = WAIT_IDLE then
        state <= WAIT_IDLE;
        READ_DATA_EN <= '0';
        CNT_TIME_EN <='0';
        DOUT_VLD <= '0';
        
           
      end if;
      
    end if;
  end process;  

end behavioral;


