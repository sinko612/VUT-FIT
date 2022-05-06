-- uart.vhd: UART controller - receiving part
-- Author(s): Simona Janosikova (xjanos19)
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

-------------------------------------------------
entity UART_RX is
port(	
  CLK      : in std_logic;
	RST      : in std_logic;
	DIN      : in std_logic;
	DOUT     : out std_logic_vector(7 downto 0);
	DOUT_VLD : out std_logic
);
end UART_RX;  

-------------------------------------------------
architecture behavioral of UART_RX is
  signal cnt_data : std_logic_vector(3 downto 0);
  signal cnt_read : std_logic_vector(4 downto 0);
  signal st_wait  : std_logic;
  signal st_data  : std_logic;
  signal vld_out  : std_logic;
  
begin
  FSM_automat: entity work.UART_FSM
  port map( 
    CLK      => CLK,
    RST      => RST,
    DIN      => DIN,
    CNT_DATA => cnt_data,
    CNT_READ => cnt_read,
    ST_WAIT  => st_wait,
    ST_DATA  => st_data,
    VLD_OUT  => vld_out
  );  
  
  process(CLK,RST)
  begin
    if (RST = '1') then
      DOUT <= "00000000";
      DOUT_VLD <= vld_out;
      cnt_data <= "0000";
      cnt_read <= "00000"; 
    elsif (CLK'event AND CLK = '1') then
      if (cnt_read = "01000") and (st_data = '0') then   
        DOUT <= "00000000";
        cnt_data <= "0000";
        cnt_read <= "00000";  
      end if;    
            
      if (st_wait = '0') then
        cnt_read <= "00000";
      else
        cnt_read <= cnt_read + 1;
      end if;

      if (st_wait = '1') and (st_data = '1') and (cnt_read = "10000") then
        if (cnt_data = "0000") then
          DOUT(0) <= DIN;
        elsif (cnt_data = "0001") then
          DOUT(1) <= DIN;
        elsif (cnt_data = "0010") then
          DOUT(2) <= DIN;
        elsif (cnt_data = "0011") then
          DOUT(3) <= DIN;
        elsif (cnt_data = "0100") then
          DOUT(4) <= DIN;
        elsif (cnt_data = "0101") then
          DOUT(5) <= DIN;
        elsif (cnt_data = "0110") then
          DOUT(6) <= DIN;
        elsif (cnt_data = "0111") then
          DOUT(7) <= DIN; 
        end if; 
        cnt_read <= "00000";   
        cnt_data <= cnt_data + '1';             
      end if;
          
      if (vld_out = '1') then
        cnt_data <= "0000";  
        cnt_read <= "00000";
        DOUT_VLD <= vld_out;
      end if;           
         
      DOUT_VLD <= vld_out;
    end if;
  end process; 
end behavioral;