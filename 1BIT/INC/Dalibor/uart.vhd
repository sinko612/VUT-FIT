-- uart.vhd: UART controller - receiving part
-- Author(s): xkrali20
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

-------------------------------------------------
entity UART_RX is
port(	
  CLK: 	    in std_logic;
	RST: 	    in std_logic;
	DIN: 	    in std_logic;
	DOUT: 	    out std_logic_vector(7 downto 0);
	DOUT_VLD: 	out std_logic
);
end UART_RX;  

-------------------------------------------------
architecture behavioral of UART_RX is
  signal cnt_data   : std_logic_vector(3 downto 0);
  signal time_count : std_logic_vector(4 downto 0);
  signal dout_vector: std_logic_vector(7 downto 0);
  signal time_bool  : std_logic;
  signal read_bool  : std_logic;
  signal vld_sig    : std_logic;
begin

  FSM_i: entity work.UART_FSM
  port map(
    CLK          => CLK,
    RST          => RST,
    DIN          => DIN,
    DOUT_VLD     => vld_sig,
    CNT_DATA     => cnt_data,
    TIME_CNT     => time_count, 
    CNT_TIME_EN  => time_bool,
    READ_DATA_EN => read_bool
  );


  process(CLK)
  begin
    if (CLK'event AND CLK = '1') then

        if ((read_bool) = '0' and time_count = "01000") then   
            time_count <= "00000";   
            dout_vector <= (others => '0');          
        end if; 

         if (RST = '1') then
            time_count <= "00000";
            cnt_data <= "0000";
            dout_vector <= "00000000";
            DOUT <= dout_vector;
            
         elsif (time_bool = '1') then
            time_count <= time_count + 1;
         end if;

         if (read_bool) = '1' and (time_count) = "10000" then
                  if (cnt_data) = "0000" then
                      dout_vector(0) <= DIN;
                  elsif (cnt_data) = "0001" then
                      dout_vector(1) <= DIN;
                  elsif (cnt_data) = "0010" then
                      dout_vector(2) <= DIN;
                  elsif (cnt_data) = "0011" then
                      dout_vector(3) <= DIN;
                  elsif (cnt_data) = "0100" then
                      dout_vector(4) <= DIN;
                  elsif (cnt_data) = "0101" then
                      dout_vector(5) <= DIN;
                  elsif (cnt_data) = "0110" then
                      dout_vector(6) <= DIN;
                  elsif (cnt_data) = "0111" then
                      dout_vector(7) <= DIN;
                  end if;    
            cnt_data <= cnt_data + '1';  
            time_count <= "00000";               
        end if;
         

         if (vld_sig) = '1' then
           DOUT <= dout_vector;
           cnt_data <= "0000";  
           time_count <= "00000";
           dout_vector <= "00000000";
         end if;           
         
         DOUT_VLD <= vld_sig;
      end if;
  end process;
end behavioral;
