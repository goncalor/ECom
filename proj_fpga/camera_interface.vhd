library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

--  Uncomment the following lines to use the declarations that are
--  provided for instantiating Xilinx primitive components.
library UNISIM;
use UNISIM.VComponents.all;

entity camera_interface is port (
	clock			: IN std_logic;
   --control
	c15 			: IN std_logic;
	c14 			: IN std_logic;
	c13 			: IN std_logic;
	c12 			: IN std_logic;
	c11 			: IN std_logic;
	c10 			: IN std_logic;
	c09 			: IN std_logic_vector(7 downto 0);        
	c08 			: OUT std_logic;
	c07 			: OUT std_logic;
	c06 			: OUT std_logic;
	c05				: OUT std_logic;
	c04				: OUT std_logic;
	c03 			: OUT std_logic_vector(7 downto 0);
	c02				: IN std_logic_vector(12 downto 0); 
	c01 			: IN std_logic;
	--FSL interface
	FSL_M_Full 		: IN std_logic;  
	FSL_M_Clk 		: OUT std_logic;
	FSL_M_Write 	: OUT std_logic;
	FSL_M_Data 		: OUT std_logic_vector(0 to 31);
	FSL_M_Control 	: OUT std_logic
   );

end camera_interface;

architecture Behavioral of camera_interface is

	COMPONENT program_regs
	PORT(
		clk         : IN std_logic;
		extern_rst  : IN std_logic;    
		sda         : OUT std_logic;      
		button      : IN std_logic;
		scl         : OUT std_logic;
		cam_rst     : OUT std_logic
		);
	END COMPONENT;

	COMPONENT image_capture
	PORT(
		CLK         : IN std_logic;
		RST         : IN std_logic;
		exclk		: OUT STD_LOGIC;
		pwdn		: OUT STD_LOGIC;
		Yi          : IN std_logic_vector(7 downto 0);
		PXL_CLK     : IN std_logic;
		HREF        : IN std_logic;
		VSYNC       : IN std_logic;
		new_frame   : out std_logic;
    	yo1 		: OUT std_logic_vector(7 downto 0);
		addr1		: IN std_logic_vector(12 downto 0); 
		en1 		: IN std_logic;
		yo2			: OUT std_logic_vector(7 downto 0); 
		addr2		: IN std_logic_vector(12 downto 0); 
		en2 		: IN std_logic 
		);
	END COMPONENT;
	
	signal 	Clk 			: std_logic;
	signal	Reset 			: std_logic;
   --control
	signal 	clkinb 			: std_logic;
	signal 	b_reset 		: std_logic;
	--camera signals
	signal 	cfg_reg 		: std_logic;
	signal 	pxl_clock 		: std_logic;
	signal 	href 			: std_logic;
	signal 	vsync 			: std_logic;
	signal 	yi 				: std_logic_vector(7 downto 0);        
	signal 	SDA 			: std_logic;
	signal 	SCL 			: std_logic;
	signal 	cam_rst 		: std_logic;
	signal 	pwdn			: std_logic;
	signal 	exclk			: std_logic;
	--captured image
	signal 	yo1 			: std_logic_vector(7 downto 0);
	signal 	addr1			: std_logic_vector(12 downto 0); 
	signal 	en1 			: std_logic;

	attribute buffer_type : string;
	attribute buffer_type of pxl_clock	: signal is "bufg";
	--attribute buffer_type of Clk		: signal is "bufg";

--FSL signals
	signal yo2			: std_logic_vector(7 downto 0); 
	signal addr2		: std_logic_vector(12 downto 0); 
	signal en2 			: std_logic;
	signal new_frame  	: std_logic;

  	type STATE_TYPE is (Idle, WRITE1, WRITE2, WRITE3, WAIT1);
  	signal CS, NS: STATE_TYPE; 

  	SIGNAL ADDRESS          : std_logic_vector(12 downto 0);
  	SIGNAL nxt_ADDRESS      : std_logic_vector(12 downto 0);
	SIGNAL DATAOUT          : std_logic_vector(7 downto 0);
  	SIGNAL nxt_FSL_M_Write  : std_logic;
	
--HIST signals
	SIGNAL HIST_Full  		: std_logic;
	SIGNAL HIST_Write  		: std_logic;
  	SIGNAL nxt_HIST_Write  	: std_logic;
	
	type HIST_STATE_TYPE is (	HIST_RESET_ST,  -- state in which the buffer is clean
								HIST_WRITE_ST, -- state in which the buffer is written 
								HIST_READ1_ST,	-- state in which the buffer is read and the order to write in the FSL released
								HIST_READ2_ST);	 -- state in which the buffer the FSL link is written
								
  	signal HIST_state: HIST_STATE_TYPE; 
	
	--'Cam' prefix is related with the side of the buffer that is connected to the camera side, and the 'MB' prefix is related with the buffer side connected to the FSL link 
	signal outCam 		: std_logic_vector(31 downto 0);
	signal outMB 		: std_logic_vector(31 downto 0);
	signal addrCam 		: std_logic_vector(8 downto 0);
	signal addrMB 		: std_logic_vector(8 downto 0);
	signal inCam 		: std_logic_vector(31 downto 0);
	signal inMB 		: std_logic_vector(31 downto 0);
	signal weCam 		: std_logic;
	signal weMB 		: std_logic;
	signal weCamDelay	: std_logic;

begin

--I/O assignments
	Clk			<= clock;
	Reset		<= c14;
   --control
	clkinb		<= c15;
	b_reset		<= c14;
	--camera signals
	cfg_reg		<= c13;
	pxl_clock 	<= c12;
	--buf_pxlc2 : BUFG port map (I => c12, O => pxl_clock);
	href 		<= c11;
	vsync 		<= c10;
	yi 			<= c09; 
	
	c08			<= SDA;
	c07			<= SCL;
	c06			<= cam_rst;
	c05			<= pwdn;
	c04			<= exclk;
	--captured image
	c03			<= yo1;
	--c03			<= X"FF";
	
	addr1		<= c02; 
	en1 		<= c01;


--Block used to program the camera registers
U2: program_regs PORT MAP(
		clk         => clkinB,
		button      => cfg_reg,
		sda         => sda,
		scl         => scl,
		extern_rst  => b_reset,
		cam_rst     => cam_rst
	);

--VGA camera image output
U3: image_capture PORT MAP(
		clk         => clkinB,
		rst         => b_reset,
		exclk       =>	exclk,
		pwdn        =>	pwdn, 
		yi          => yi,
		pxl_clk     => pxl_clock,
		href        => href,
		vsync       => vsync,
		new_frame   => new_frame,
		yo1     	=> yo1,
		addr1     	=> addr1,
		en1     	=> en1,
		yo2      	=> yo2,
		addr2     	=> addr2,
		en2     	=> en2
	);

--In order to manage the hist buffer, we must replace the FSL signals with the HIST signals in the SYNC_PROC and COMB_PROC processes
--since these processes do not longer manage the FSL link	
--FSL state machine
	DATAOUT <= yo2;
	addr2	<= ADDRESS;
	en2 <= '1';

   SYNC_PROC: process (Clk, Reset)
    begin
      if (Reset='1') then
        CS            <= Idle;
        HIST_Write    <= '0';
        ADDRESS       <= (others => '0');
      elsif (Clk'event and Clk = '1') then
        CS            <= NS;
        ADDRESS       <= nxt_ADDRESS;
        HIST_Write    <= nxt_HIST_Write;
      end if;
    end process;
	
    COMB_PROC: process (CS, HIST_Full, ADDRESS, New_Frame)
    begin
       NS                 <= CS;   
       nxt_HIST_Write     <= '0';
       nxt_ADDRESS        <= ADDRESS;
       case CS is
            when Idle        =>
                nxt_ADDRESS <= (others =>'0');
                if (New_Frame ='1') then
                    NS <= WRITE1;  
                end if;

            when WRITE1 =>
                  if (HIST_Full='0') then
                    NS              <= WRITE2;
                  end if;
         
            when WRITE2 =>        
                  NS              <= WRITE3;
                  if (HIST_Full='0') then
                    nxt_HIST_Write <= '1';
                  end if; 

            when WRITE3 =>
                  NS              <= WRITE1;
                  nxt_ADDRESS     <= ADDRESS + 1;
                  if (ADDRESS = "1111111111111") then
                      NS              <= WAIT1;
                  end if;

            when WAIT1  => 
                if (New_Frame ='0') then
                    NS <= Idle;  
               end if;

            when others         =>
                  NS  <= idle; 
       end case;
    end process; 
    
	--The FSL_M_Data is no longer manage by the DATAOUT signal which comes directly from the camera
    --FSL_M_Data    <= X"000000" & DATAOUT;
    FSL_M_Control <= '0';
    FSL_M_Clk     <= Clk;

	--Now we will create the buffer to store the histogram and to control it
	
	--This is a RAM component available in the FPGAs
	HIST_buffer : RAMB16_S36_S36
   port map (
      DOA => outCam,      	-- Port A 32-bit Data Output
      DOB => outMB,      	-- Port B 32-bit Data Output
      DOPA => open,    		-- Port A 4-bit Parity Output
      DOPB => open,    		-- Port B 4-bit Parity Output
      ADDRA => addrCam,		-- Port A 9-bit Address Input
      ADDRB => addrMB,  	-- Port B 9-bit Address Input
      CLKA => Clk,    		-- Port A Clock
      CLKB => Clk,    		-- Port B Clock
      DIA => inCam,      	-- Port A 32-bit Data Input
      DIB => inMB,      	-- Port B 32-bit Data Input
      DIPA => "0000",    	-- Port A 4-bit parity Input
      DIPB => "0000",    	-- Port-B 4-bit parity Input
      ENA => '1',      		-- Port A RAM Enable Input
      ENB => '1',      		-- PortB RAM Enable Input
      SSRA => '0',    		-- Port A Synchronous Set/Reset Input
      SSRB => '0',    		-- Port B Synchronous Set/Reset Input
      WEA => weCam,      	-- Port A Write Enable Input
      WEB => weMB       	-- Port B Write Enable Input
   );
   
   --the camera side of the buffer consists to increments
   inCam 		<= X"0000" & "000" & (outCam(12 downto 0) + '1');
   
   --the MB side of the buffer connects to the FSL link
   FSL_M_Data 	<= outMB;
   
   --the MB side of the buffer input is used to clean the buffer
   inMB			<= X"00000000";
   
	--process to handle the synchronous signals
	HIST_PROC: process (Clk)
    begin
		if rising_edge(Clk) then -- process is syncronous
			if Reset = '1' then
				HIST_Full 	<= '1'; --while this signal is set the camera related ahrdware does not send data
				FSL_M_Write	<= '0';
				weCam		<= '0';
				weCamDelay	<= '0'; -- this signal introduces a delay in the write enable of the buffer since, we need to read before writting
				weMB		<= '0';
				addrCam		<= "000000000";
				addrMB		<= "000000000";
				HIST_state	<= HIST_RESET_ST;
			else
				weCam <= weCamDelay;
				
				case HIST_state is
					when HIST_RESET_ST		=>
						HIST_Full 	<= '1';
						FSL_M_Write	<= '0';
						weCamDelay	<= '0';
						weMB		<= '1';
						addrCam		<= "000000000";
						
						if addrMB = "001111111" then --all addresses are already clear
							addrMB		<= "000000000";
							HIST_state	<= HIST_WRITE_ST;
						else
							addrMB		<= addrMB + '1';
							HIST_state	<= HIST_RESET_ST;
						end if;
						
					when HIST_WRITE_ST	=>
						addrMB		<= "000000000";
						FSL_M_Write	<= '0';
						weMB		<= '0';
						weCamDelay	<= HIST_Write; --we propagate (and delay) the write enable every time it comes from the camera hardware
						
						if HIST_Write = '1' then
							addrCam <= "00" & DATAOUT(7 downto 1); 
						
							if ADDRESS = "1111111111111" then --the complete image was communicated
								HIST_Full 	<= '1'; 
								HIST_state	<= HIST_READ1_ST;
							else
								HIST_Full 	<= '0'; 
								HIST_state	<= HIST_WRITE_ST;
							end if;
						else 
							addrCam <= addrCam;
							HIST_Full 	<= '0'; 
							HIST_state	<= HIST_WRITE_ST;
						end if;

					when HIST_READ1_ST 		=>
						weMB		<= '0';
						weCamDelay	<= '0';
						addrCam 	<= "000000000"; 
						HIST_Full 	<= '1'; 
						addrMB		<= addrMB;

						if FSL_M_Full = '0' then
							FSL_M_Write	<= '1';
							HIST_state	<= HIST_READ2_ST;
						else
							FSL_M_Write	<= '0';
							HIST_state	<= HIST_READ1_ST;
						end if;


					when HIST_READ2_ST 	=>
						weMB		<= '0';
						weCamDelay	<= '0';
						addrCam 	<= "000000000"; 
						HIST_Full 	<= '1'; 
						FSL_M_Write	<= '0';
							
						if addrMB = "001111111" then --last value is comunicated to the histogram
							addrMB <= "000000000";
							HIST_state	<= HIST_RESET_ST;
						else
							addrMB 		<= addrMB + '1';
							HIST_state	<= HIST_READ1_ST;
						end if;
					
					when others         =>
						  HIST_state  <= HIST_RESET_ST; 
				end case;
			end if;	
		  end if;
    end process;

end Behavioral;
