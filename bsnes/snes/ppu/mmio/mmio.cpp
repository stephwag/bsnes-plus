#ifdef PPU_CPP

bool PPU::interlace() const {
  return display.interlace;
}

bool PPU::overscan() const {
  return display.overscan;
}

bool PPU::hires() const {
  return true;
}

void PPU::latch_counters() {
  cpu.synchronize_ppu();
  regs.hcounter = hdot();
  regs.vcounter = vcounter();
  regs.counters_latched = true;
}

unsigned PPU::get_vram_address() {
  unsigned addr = regs.vram_addr;
  switch(regs.vram_mapping) {
    case 0: break;  //direct mapping
    case 1: addr = (addr & 0xff00) | ((addr & 0x001f) << 3) | ((addr >> 5) & 7); break;
    case 2: addr = (addr & 0xfe00) | ((addr & 0x003f) << 3) | ((addr >> 6) & 7); break;
    case 3: addr = (addr & 0xfc00) | ((addr & 0x007f) << 3) | ((addr >> 7) & 7); break;
  }
  return (addr << 1);
}

uint8 PPU::vram_read(unsigned addr) {
  if(regs.display_disable || vcounter() >= (!regs.overscan ? 225 : 240)) {
    return memory::vram[addr];
  }
  return 0x00;
}

void PPU::vram_write(unsigned addr, uint8 data) {
  if(regs.display_disable || vcounter() >= (!regs.overscan ? 225 : 240)) {
    memory::vram[addr] = data;
  }
}

uint8 PPU::oam_read(unsigned addr) {
  // address is handled by mmio_r2138
  return memory::oam[addr];
}

void PPU::oam_write(unsigned addr, uint8 data) {
  // address/data are handled by mmio_w2104
  oam.update(addr, data);
}

uint8 PPU::cgram_read(unsigned addr) {
  // address is handled by mmio_r213b
  return memory::cgram[addr];
}

void PPU::cgram_write(unsigned addr, uint8 data) {
  // address/data are handled by mmio_w2122
  memory::cgram[addr] = data;
}

void PPU::mmio_update_video_mode() {
  switch(regs.bgmode) {
    case 0: {
      bg1.regs.mode = Background::Mode::BPP2; bg1.regs.priority0 = 8; bg1.regs.priority1 = 11;
      bg2.regs.mode = Background::Mode::BPP2; bg2.regs.priority0 = 7; bg2.regs.priority1 = 10;
      bg3.regs.mode = Background::Mode::BPP2; bg3.regs.priority0 = 2; bg3.regs.priority1 =  5;
      bg4.regs.mode = Background::Mode::BPP2; bg4.regs.priority0 = 1; bg4.regs.priority1 =  4;
      oam.regs.priority0 = 3; oam.regs.priority1 = 6; oam.regs.priority2 = 9; oam.regs.priority3 = 12;
    } break;

    case 1: {
      bg1.regs.mode = Background::Mode::BPP4;
      bg2.regs.mode = Background::Mode::BPP4;
      bg3.regs.mode = Background::Mode::BPP2;
      bg4.regs.mode = Background::Mode::Inactive;
      if(regs.bg3_priority) {
        bg1.regs.priority0 = 5; bg1.regs.priority1 =  8;
        bg2.regs.priority0 = 4; bg2.regs.priority1 =  7;
        bg3.regs.priority0 = 1; bg3.regs.priority1 = 10;
        oam.regs.priority0 = 2; oam.regs.priority1 = 3; oam.regs.priority2 = 6; oam.regs.priority3 = 9;
      } else {
        bg1.regs.priority0 = 6; bg1.regs.priority1 =  9;
        bg2.regs.priority0 = 5; bg2.regs.priority1 =  8;
        bg3.regs.priority0 = 1; bg3.regs.priority1 =  3;
        oam.regs.priority0 = 2; oam.regs.priority1 = 4; oam.regs.priority2 = 7; oam.regs.priority3 = 10;
      }
    } break;

    case 2: {
      bg1.regs.mode = Background::Mode::BPP4;
      bg2.regs.mode = Background::Mode::BPP4;
      bg3.regs.mode = Background::Mode::Inactive;
      bg4.regs.mode = Background::Mode::Inactive;
      bg1.regs.priority0 = 3; bg1.regs.priority1 = 7;
      bg2.regs.priority0 = 1; bg2.regs.priority1 = 5;
      oam.regs.priority0 = 2; oam.regs.priority1 = 4; oam.regs.priority2 = 6; oam.regs.priority3 = 8;
    } break;

    case 3: {
      bg1.regs.mode = Background::Mode::BPP8;
      bg2.regs.mode = Background::Mode::BPP4;
      bg3.regs.mode = Background::Mode::Inactive;
      bg4.regs.mode = Background::Mode::Inactive;
      bg1.regs.priority0 = 3; bg1.regs.priority1 = 7;
      bg2.regs.priority0 = 1; bg2.regs.priority1 = 5;
      oam.regs.priority0 = 2; oam.regs.priority1 = 4; oam.regs.priority2 = 6; oam.regs.priority3 = 8;
    } break;

    case 4: {
      bg1.regs.mode = Background::Mode::BPP8;
      bg2.regs.mode = Background::Mode::BPP2;
      bg3.regs.mode = Background::Mode::Inactive;
      bg4.regs.mode = Background::Mode::Inactive;
      bg1.regs.priority0 = 3; bg1.regs.priority1 = 7;
      bg2.regs.priority0 = 1; bg2.regs.priority1 = 5;
      oam.regs.priority0 = 2; oam.regs.priority1 = 4; oam.regs.priority2 = 6; oam.regs.priority3 = 8;
    } break;

    case 5: {
      bg1.regs.mode = Background::Mode::BPP4;
      bg2.regs.mode = Background::Mode::BPP2;
      bg3.regs.mode = Background::Mode::Inactive;
      bg4.regs.mode = Background::Mode::Inactive;
      bg1.regs.priority0 = 3; bg1.regs.priority1 = 7;
      bg2.regs.priority0 = 1; bg2.regs.priority1 = 5;
      oam.regs.priority0 = 2; oam.regs.priority1 = 4; oam.regs.priority2 = 6; oam.regs.priority3 = 8;
    } break;

    case 6: {
      bg1.regs.mode = Background::Mode::BPP4;
      bg2.regs.mode = Background::Mode::Inactive;
      bg3.regs.mode = Background::Mode::Inactive;
      bg4.regs.mode = Background::Mode::Inactive;
      bg1.regs.priority0 = 2; bg1.regs.priority1 = 5;
      oam.regs.priority0 = 1; oam.regs.priority1 = 3; oam.regs.priority2 = 4; oam.regs.priority3 = 6;
    } break;

    case 7: {
      if(regs.mode7_extbg == false) {
        bg1.regs.mode = Background::Mode::Mode7;
        bg2.regs.mode = Background::Mode::Inactive;
        bg3.regs.mode = Background::Mode::Inactive;
        bg4.regs.mode = Background::Mode::Inactive;
        bg1.regs.priority0 = 2; bg1.regs.priority1 = 2;
        oam.regs.priority0 = 1; oam.regs.priority1 = 3; oam.regs.priority2 = 4; oam.regs.priority3 = 5;
      } else {
        bg1.regs.mode = Background::Mode::Mode7;
        bg2.regs.mode = Background::Mode::Mode7;
        bg3.regs.mode = Background::Mode::Inactive;
        bg4.regs.mode = Background::Mode::Inactive;
        bg1.regs.priority0 = 3; bg1.regs.priority1 = 3;
        bg2.regs.priority0 = 1; bg2.regs.priority1 = 5;
        oam.regs.priority0 = 2; oam.regs.priority1 = 4; oam.regs.priority2 = 6; oam.regs.priority3 = 7;
      }
    } break;
  }
}

//INIDISP
void PPU::mmio_w2100(uint8 data) {
  if(regs.display_disable && vcounter() == (!regs.overscan ? 225 : 240)) oam.address_reset();
  regs.display_disable = data & 0x80;
  regs.display_brightness = data & 0x0f;
}

//OBSEL
void PPU::mmio_w2101(uint8 data) {
  oam.regs.base_size = (data >> 5) & 7;
  oam.regs.nameselect = (data >> 3) & 3;
  oam.regs.tiledata_addr = (data & 7) << 14;
}

//OAMADDL
void PPU::mmio_w2102(uint8 data) {
  regs.oam_baseaddr = (regs.oam_baseaddr & 0x0600) | (data << 1);
  oam.address_reset();
}

//OAMADDH
void PPU::mmio_w2103(uint8 data) {
  regs.oam_priority = data & 0x80;
  regs.oam_baseaddr = ((data & 0x01) << 9) | (regs.oam_baseaddr & 0x01fe);
  oam.address_reset();
}

//OAMDATA
void PPU::mmio_w2104(uint8 data) {
  bool latch = regs.oam_addr & 1;
  uint10 addr = regs.oam_addr++;
  if(regs.display_disable == false && vcounter() < (!regs.overscan ? 225 : 240)) addr = regs.oam_iaddr;
  if(addr & 0x0200) addr &= 0x021f;

  if(latch == 0) regs.oam_latchdata = data;
  if(addr & 0x0200) {
    oam_write(addr, data);
  } else if(latch == 1) {
    oam_write((addr & ~1) + 0, regs.oam_latchdata);
    oam_write((addr & ~1) + 1, data);
  }
  oam.set_first_sprite();
}

//BGMODE
void PPU::mmio_w2105(uint8 data) {
  bg4.regs.tile_size = (data & 0x80);
  bg3.regs.tile_size = (data & 0x40);
  bg2.regs.tile_size = (data & 0x20);
  bg1.regs.tile_size = (data & 0x10);
  regs.bg3_priority = (data & 0x08);
  regs.bgmode = (data & 0x07);
  mmio_update_video_mode();
}

//MOSAIC
void PPU::mmio_w2106(uint8 data) {
  unsigned mosaic_size = (data >> 4) & 15;
  bg4.regs.mosaic = (data & 0x08 ? mosaic_size : 0);
  bg3.regs.mosaic = (data & 0x04 ? mosaic_size : 0);
  bg2.regs.mosaic = (data & 0x02 ? mosaic_size : 0);
  bg1.regs.mosaic = (data & 0x01 ? mosaic_size : 0);
}

//BG1SC
void PPU::mmio_w2107(uint8 data) {
  bg1.regs.screen_addr = (data & 0xfc) << 9;
  bg1.regs.screen_size = data & 3;
}

//BG2SC
void PPU::mmio_w2108(uint8 data) {
  bg2.regs.screen_addr = (data & 0xfc) << 9;
  bg2.regs.screen_size = data & 3;
}

//BG3SC
void PPU::mmio_w2109(uint8 data) {
  bg3.regs.screen_addr = (data & 0xfc) << 9;
  bg3.regs.screen_size = data & 3;
}

//BG4SC
void PPU::mmio_w210a(uint8 data) {
  bg4.regs.screen_addr = (data & 0xfc) << 9;
  bg4.regs.screen_size = data & 3;
}

//BG12NBA
void PPU::mmio_w210b(uint8 data) {
  bg1.regs.tiledata_addr = (data & 0x0f) << 13;
  bg2.regs.tiledata_addr = (data & 0xf0) <<  9;
}

//BG34NBA
void PPU::mmio_w210c(uint8 data) {
  bg3.regs.tiledata_addr = (data & 0x0f) << 13;
  bg4.regs.tiledata_addr = (data & 0xf0) <<  9;
}

//BG1HOFS
void PPU::mmio_w210d(uint8 data) {
  regs.mode7_hoffset = (data << 8) | regs.mode7_latchdata;
  regs.mode7_latchdata = data;

  bg1.regs.hoffset = (data << 8) | (regs.bgofs_ppu1latchdata & ~7) | (regs.bgofs_ppu2latchdata);
  regs.bgofs_ppu1latchdata = data;
  regs.bgofs_ppu2latchdata = data;
}

//BG1VOFS
void PPU::mmio_w210e(uint8 data) {
  regs.mode7_voffset = (data << 8) | regs.mode7_latchdata;
  regs.mode7_latchdata = data;

  bg1.regs.voffset = (data << 8) | regs.bgofs_ppu1latchdata;
  regs.bgofs_ppu1latchdata = data;
}

//BG2HOFS
void PPU::mmio_w210f(uint8 data) {
  bg2.regs.hoffset = (data << 8) | (regs.bgofs_ppu1latchdata & ~7) | (regs.bgofs_ppu2latchdata);
  regs.bgofs_ppu1latchdata = data;
  regs.bgofs_ppu2latchdata = data;
}

//BG2VOFS
void PPU::mmio_w2110(uint8 data) {
  bg2.regs.voffset = (data << 8) | regs.bgofs_ppu1latchdata;
  regs.bgofs_ppu1latchdata = data;
}

//BG3HOFS
void PPU::mmio_w2111(uint8 data) {
  bg3.regs.hoffset = (data << 8) | (regs.bgofs_ppu1latchdata & ~7) | (regs.bgofs_ppu2latchdata);
  regs.bgofs_ppu1latchdata = data;
  regs.bgofs_ppu2latchdata = data;
}

//BG3VOFS
void PPU::mmio_w2112(uint8 data) {
  bg3.regs.voffset = (data << 8) | regs.bgofs_ppu1latchdata;
  regs.bgofs_ppu1latchdata = data;
}

//BG4HOFS
void PPU::mmio_w2113(uint8 data) {
  bg4.regs.hoffset = (data << 8) | (regs.bgofs_ppu1latchdata & ~7) | (regs.bgofs_ppu2latchdata);
  regs.bgofs_ppu1latchdata = data;
  regs.bgofs_ppu2latchdata = data;
}

//BG4VOFS
void PPU::mmio_w2114(uint8 data) {
  bg4.regs.voffset = (data << 8) | regs.bgofs_ppu1latchdata;
  regs.bgofs_ppu1latchdata = data;
}

//VMAIN
void PPU::mmio_w2115(uint8 data) {
  regs.vram_incmode = data & 0x80;
  regs.vram_mapping = (data >> 2) & 3;
  switch(data & 3) {
    case 0: regs.vram_incsize =   1; break;
    case 1: regs.vram_incsize =  32; break;
    case 2: regs.vram_incsize = 128; break;
    case 3: regs.vram_incsize = 128; break;
  }
}

//VMADDL
void PPU::mmio_w2116(uint8 data) {
  regs.vram_addr &= 0xff00;
  regs.vram_addr |= (data << 0);
  unsigned addr = get_vram_address();
  regs.vram_readbuffer  = vram_read(addr + 0) << 0;
  regs.vram_readbuffer |= vram_read(addr + 1) << 8;
}

//VMADDH
void PPU::mmio_w2117(uint8 data) {
  regs.vram_addr &= 0x00ff;
  regs.vram_addr |= (data << 8);
  unsigned addr = get_vram_address();
  regs.vram_readbuffer  = vram_read(addr + 0) << 0;
  regs.vram_readbuffer |= vram_read(addr + 1) << 8;
}

//VMDATAL
void PPU::mmio_w2118(uint8 data) {
  unsigned addr = get_vram_address() + 0;
  vram_write(addr, data);
  if(regs.vram_incmode == 0) regs.vram_addr += regs.vram_incsize;
}

//VMDATAH
void PPU::mmio_w2119(uint8 data) {
  unsigned addr = get_vram_address() + 1;
  vram_write(addr, data);
  if(regs.vram_incmode == 1) regs.vram_addr += regs.vram_incsize;
}

//M7SEL
void PPU::mmio_w211a(uint8 data) {
  regs.mode7_repeat = (data >> 6) & 3;
  regs.mode7_vflip = data & 0x02;
  regs.mode7_hflip = data & 0x01;
}

//M7A
void PPU::mmio_w211b(uint8 data) {
  regs.m7a = (data << 8) | regs.mode7_latchdata;
  regs.mode7_latchdata = data;
}

//M7B
void PPU::mmio_w211c(uint8 data) {
  regs.m7b = (data << 8) | regs.mode7_latchdata;
  regs.mode7_latchdata = data;
}

//M7C
void PPU::mmio_w211d(uint8 data) {
  regs.m7c = (data << 8) | regs.mode7_latchdata;
  regs.mode7_latchdata = data;
}

//M7D
void PPU::mmio_w211e(uint8 data) {
  regs.m7d = (data << 8) | regs.mode7_latchdata;
  regs.mode7_latchdata = data;
}

//M7X
void PPU::mmio_w211f(uint8 data) {
  regs.m7x = (data << 8) | regs.mode7_latchdata;
  regs.mode7_latchdata = data;
}

//M7Y
void PPU::mmio_w2120(uint8 data) {
  regs.m7y = (data << 8) | regs.mode7_latchdata;
  regs.mode7_latchdata = data;
}

//CGADD
void PPU::mmio_w2121(uint8 data) {
  regs.cgram_addr = data << 1;
}

//CGDATA
void PPU::mmio_w2122(uint8 data) {
  bool latch = regs.cgram_addr & 1;
  uint9 addr = regs.cgram_addr++;
  if(regs.display_disable == false
  && vcounter() > 0 && vcounter() < (!regs.overscan ? 225 : 240)
  && hcounter() >= 88 && hcounter() < 1096
  ) addr = regs.cgram_iaddr;

  if(latch == 0) {
    regs.cgram_latchdata = data;
  } else {
    cgram_write((addr & ~1) + 0, regs.cgram_latchdata);
    cgram_write((addr & ~1) + 1, data & 0x7f);
  }
}

//W12SEL
void PPU::mmio_w2123(uint8 data) {
  window.regs.bg2_two_enable = data & 0x80;
  window.regs.bg2_two_invert = data & 0x40;
  window.regs.bg2_one_enable = data & 0x20;
  window.regs.bg2_one_invert = data & 0x10;
  window.regs.bg1_two_enable = data & 0x08;
  window.regs.bg1_two_invert = data & 0x04;
  window.regs.bg1_one_enable = data & 0x02;
  window.regs.bg1_one_invert = data & 0x01;
}

//W34SEL
void PPU::mmio_w2124(uint8 data) {
  window.regs.bg4_two_enable = data & 0x80;
  window.regs.bg4_two_invert = data & 0x40;
  window.regs.bg4_one_enable = data & 0x20;
  window.regs.bg4_one_invert = data & 0x10;
  window.regs.bg3_two_enable = data & 0x08;
  window.regs.bg3_two_invert = data & 0x04;
  window.regs.bg3_one_enable = data & 0x02;
  window.regs.bg3_one_invert = data & 0x01;
}

//WOBJSEL
void PPU::mmio_w2125(uint8 data) {
  window.regs.col_two_enable = data & 0x80;
  window.regs.col_two_invert = data & 0x40;
  window.regs.col_one_enable = data & 0x20;
  window.regs.col_one_invert = data & 0x10;
  window.regs.oam_two_enable = data & 0x08;
  window.regs.oam_two_invert = data & 0x04;
  window.regs.oam_one_enable = data & 0x02;
  window.regs.oam_one_invert = data & 0x01;
}

//WH0
void PPU::mmio_w2126(uint8 data) {
  window.regs.one_left = data;
}

//WH1
void PPU::mmio_w2127(uint8 data) {
  window.regs.one_right = data;
}

//WH2
void PPU::mmio_w2128(uint8 data) {
  window.regs.two_left = data;
}

//WH3
void PPU::mmio_w2129(uint8 data) {
  window.regs.two_right = data;
}

//WBGLOG
void PPU::mmio_w212a(uint8 data) {
  window.regs.bg4_mask = (data >> 6) & 3;
  window.regs.bg3_mask = (data >> 4) & 3;
  window.regs.bg2_mask = (data >> 2) & 3;
  window.regs.bg1_mask = (data >> 0) & 3;
}

//WOBJLOG
void PPU::mmio_w212b(uint8 data) {
  window.regs.col_mask = (data >> 2) & 3;
  window.regs.oam_mask = (data >> 0) & 3;
}

//TM
void PPU::mmio_w212c(uint8 data) {
  oam.regs.main_enable = data & 0x10;
  bg4.regs.main_enable = data & 0x08;
  bg3.regs.main_enable = data & 0x04;
  bg2.regs.main_enable = data & 0x02;
  bg1.regs.main_enable = data & 0x01;
}

//TS
void PPU::mmio_w212d(uint8 data) {
  oam.regs.sub_enable = data & 0x10;
  bg4.regs.sub_enable = data & 0x08;
  bg3.regs.sub_enable = data & 0x04;
  bg2.regs.sub_enable = data & 0x02;
  bg1.regs.sub_enable = data & 0x01;
}

//TMW
void PPU::mmio_w212e(uint8 data) {
  window.regs.oam_main_enable = data & 0x10;
  window.regs.bg4_main_enable = data & 0x08;
  window.regs.bg3_main_enable = data & 0x04;
  window.regs.bg2_main_enable = data & 0x02;
  window.regs.bg1_main_enable = data & 0x01;
}

//TSW
void PPU::mmio_w212f(uint8 data) {
  window.regs.oam_sub_enable = data & 0x10;
  window.regs.bg4_sub_enable = data & 0x08;
  window.regs.bg3_sub_enable = data & 0x04;
  window.regs.bg2_sub_enable = data & 0x02;
  window.regs.bg1_sub_enable = data & 0x01;
}

//CGWSEL
void PPU::mmio_w2130(uint8 data) {
  window.regs.col_main_mask = (data >> 6) & 3;
  window.regs.col_sub_mask = (data >> 4) & 3;
  screen.regs.addsub_mode = data & 0x02;
  screen.regs.direct_color = data & 0x01;
}

//CGADDSUB
void PPU::mmio_w2131(uint8 data) {
  screen.regs.color_mode = data & 0x80;
  screen.regs.color_halve = data & 0x40;
  screen.regs.back_color_enable = data & 0x20;
  screen.regs.oam_color_enable = data & 0x10;
  screen.regs.bg4_color_enable = data & 0x08;
  screen.regs.bg3_color_enable = data & 0x04;
  screen.regs.bg2_color_enable = data & 0x02;
  screen.regs.bg1_color_enable = data & 0x01;
}

//COLDATA
void PPU::mmio_w2132(uint8 data) {
  unsigned new_color = data & 0x1f;
  unsigned old_color = screen.regs.color;
  unsigned red   = data & 0x20 ? new_color << 0  : old_color & 0x1f;
  unsigned green = data & 0x40 ? new_color << 5  : old_color & (0x1f << 5);
  unsigned blue  = data & 0x80 ? new_color << 10 : old_color & (0x1f << 10);
  screen.regs.color = red | green | blue;
}

//SETINI
void PPU::mmio_w2133(uint8 data) {
  regs.mode7_extbg = data & 0x40;
  regs.pseudo_hires = data & 0x08;
  regs.overscan = data & 0x04;
  oam.regs.interlace = data & 0x02;
  regs.interlace = data & 0x01;
  mmio_update_video_mode();
}

//MPYL
uint8 PPU::mmio_r2134() {
  unsigned result = ((int16)regs.m7a * (int8)(regs.m7b >> 8));
  if(Memory::debugger_access())
    return result >> 0;
  
  regs.ppu1_mdr = (result >>  0);
  return regs.ppu1_mdr;
}

//MPYM
uint8 PPU::mmio_r2135() {
  unsigned result = ((int16)regs.m7a * (int8)(regs.m7b >> 8));
  if(Memory::debugger_access())
    return result >> 8;
  
  regs.ppu1_mdr = (result >>  8);
  return regs.ppu1_mdr;
}

//MPYH
uint8 PPU::mmio_r2136() {
  unsigned result = ((int16)regs.m7a * (int8)(regs.m7b >> 8));
  if(Memory::debugger_access())
    return result >> 16;
    
  regs.ppu1_mdr = (result >> 16);
  return regs.ppu1_mdr;
}

//SLHV
uint8 PPU::mmio_r2137() {
  if(!Memory::debugger_access() && (cpu.pio() & 0x80)) latch_counters();
  return cpu.regs.mdr;
}

//OAMDATAREAD
uint8 PPU::mmio_r2138() {
  uint10 addr = regs.oam_addr;
  if(regs.display_disable == false && vcounter() < (!regs.overscan ? 225 : 240)) addr = regs.oam_iaddr;
  if(addr & 0x0200) addr &= 0x021f;

  if(Memory::debugger_access())
    return oam_read(addr);

  regs.oam_addr++;
  regs.ppu1_mdr = oam_read(addr);
  oam.set_first_sprite();
  return regs.ppu1_mdr;
}

//VMDATALREAD
uint8 PPU::mmio_r2139() {
  if(Memory::debugger_access())
    return regs.vram_readbuffer >> 0;
  
  unsigned addr = get_vram_address() + 0;
  
  regs.ppu1_mdr = regs.vram_readbuffer >> 0;
  if(regs.vram_incmode == 0) {
    addr &= ~1;
    regs.vram_readbuffer  = vram_read(addr + 0) << 0;
    regs.vram_readbuffer |= vram_read(addr + 1) << 8;
    regs.vram_addr += regs.vram_incsize;
  }
  return regs.ppu1_mdr;
}

//VMDATAHREAD
uint8 PPU::mmio_r213a() {
  if(Memory::debugger_access())
    return regs.vram_readbuffer >> 8;
    
  unsigned addr = get_vram_address() + 1;
    
  regs.ppu1_mdr = regs.vram_readbuffer >> 8;
  if(regs.vram_incmode == 1) {
    addr &= ~1;
    regs.vram_readbuffer  = vram_read(addr + 0) << 0;
    regs.vram_readbuffer |= vram_read(addr + 1) << 8;
    regs.vram_addr += regs.vram_incsize;
  }
  return regs.ppu1_mdr;
}

//CGDATAREAD
uint8 PPU::mmio_r213b() {
  bool latch = regs.cgram_addr & 1;
  uint9 addr = regs.cgram_addr;
  if(regs.display_disable == false
  && vcounter() > 0 && vcounter() < (!regs.overscan ? 225 : 240)
  && hcounter() >= 88 && hcounter() < 1096
  ) addr = regs.cgram_iaddr;

  uint8 r = regs.ppu2_mdr;

  if(latch == 0) {
    r  = cgram_read(addr);
  } else {
    r &= 0x80;
    r |= cgram_read(addr);
  }
  
  if(Memory::debugger_access())
    return r;
  
  regs.ppu2_mdr = r;
  regs.cgram_addr++;
  return regs.ppu2_mdr;
}

//OPHCT
uint8 PPU::mmio_r213c() {
  uint8 r = regs.ppu2_mdr;

  if(regs.latch_hcounter == 0) {
    r  = (regs.hcounter >> 0);
  } else {
    r &= 0xfe;
    r |= (regs.hcounter >> 8) & 1;
  }
  
  if(Memory::debugger_access())
    return r;
  
  regs.ppu2_mdr = r;
  regs.latch_hcounter ^= 1;
  return regs.ppu2_mdr;
}

//OPVCT
uint8 PPU::mmio_r213d() {
  uint8 r = regs.ppu2_mdr;

  if(regs.latch_vcounter == 0) {
    r  = (regs.vcounter >> 0);
  } else {
    r &= 0xfe;
    r |= (regs.vcounter >> 8) & 1;
  }
  
  if(Memory::debugger_access())
    return r;
  
  regs.ppu2_mdr = r;
  regs.latch_vcounter ^= 1;
  return regs.ppu2_mdr;
}

//STAT77
uint8 PPU::mmio_r213e() {
  uint8 r = regs.ppu1_mdr;

  r &= 0x10;
  r |= oam.regs.time_over << 7;
  r |= oam.regs.range_over << 6;
  r |= ppu1_version & 0x0f;
  
  if(Memory::debugger_access())
    return r;
  
  regs.ppu1_mdr = r;
  return regs.ppu1_mdr;
}

//STAT78
uint8 PPU::mmio_r213f() {
  uint8 r = regs.ppu2_mdr;

  if(!Memory::debugger_access()) {
    regs.latch_hcounter = 0;
    regs.latch_vcounter = 0;
  }

  r &= 0x20;
  r |= field() << 7;
  if((cpu.pio() & 0x80) == 0) {
    r |= 0x40;
  } else if(regs.counters_latched) {
    r |= 0x40;
    if(!Memory::debugger_access())
      regs.counters_latched = false;
  }
  r |= (system.region() == System::Region::NTSC ? 0 : 1) << 4;
  r |= ppu2_version & 0x0f;
  
  if(Memory::debugger_access())
    return r;
  
  regs.ppu2_mdr = r;
  return regs.ppu2_mdr;
}

void PPU::mmio_reset() {
  regs.ppu1_mdr = random(0xff);
  regs.ppu2_mdr = random(0xff);

  regs.vram_readbuffer = random(0x0000);
  regs.oam_latchdata = random(0x00);
  regs.cgram_latchdata = random(0x00);
  regs.bgofs_ppu1latchdata = random(0x00);
  regs.bgofs_ppu2latchdata = random(0x00);
  regs.mode7_latchdata = random(0x00);
  regs.counters_latched = false;
  regs.latch_hcounter = 0;
  regs.latch_vcounter = 0;

  regs.oam_iaddr = 0x0000;
  regs.cgram_iaddr = 0x00;

  //$2100  INIDISP
  regs.display_disable = true;
  regs.display_brightness = 0;

  //$2102  OAMADDL
  //$2103  OAMADDH
  regs.oam_baseaddr = random(0x0000);
  regs.oam_addr = random(0x0000);
  regs.oam_priority = random(false);

  //$2105  BGMODE
  regs.bg3_priority = false;
  regs.bgmode = 0;

  //$210d  BG1HOFS
  regs.mode7_hoffset = random(0x0000);

  //$210e  BG1VOFS
  regs.mode7_voffset = random(0x0000);

  //$2115  VMAIN
  regs.vram_incmode = random(1);
  regs.vram_mapping = random(0);
  regs.vram_incsize = 1;

  //$2116  VMADDL
  //$2117  VMADDH
  regs.vram_addr = random(0x0000);

  //$211a  M7SEL
  regs.mode7_repeat = random(0);
  regs.mode7_vflip = random(false);
  regs.mode7_hflip = random(false);

  //$211b  M7A
  regs.m7a = random(0x0000);

  //$211c  M7B
  regs.m7b = random(0x0000);

  //$211d  M7C
  regs.m7c = random(0x0000);

  //$211e  M7D
  regs.m7d = random(0x0000);

  //$211f  M7X
  regs.m7x = random(0x0000);

  //$2120  M7Y
  regs.m7y = random(0x0000);

  //$2121  CGADD
  regs.cgram_addr = random(0x0000);

  //$2133  SETINI
  regs.mode7_extbg = random(false);
  regs.pseudo_hires = random(false);
  regs.overscan = false;
  regs.interlace = false;

  //$213c  OPHCT
  regs.hcounter = 0;

  //$213d  OPVCT
  regs.vcounter = 0;
}

uint8 PPU::mmio_read(unsigned addr) {
  if(!Memory::debugger_access())
    cpu.synchronize_ppu();

  switch(addr & 0x3f) {
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x08:
    case 0x09:
    case 0x0a:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x18:
    case 0x19:
    case 0x1a:
    case 0x24:
    case 0x25:
    case 0x26:
    case 0x28:
    case 0x29:
    case 0x2a: return regs.ppu1_mdr;
    case 0x34: return mmio_r2134();  //MPYL
    case 0x35: return mmio_r2135();  //MPYM
    case 0x36: return mmio_r2136();  //MYPH
    case 0x37: return mmio_r2137();  //SLHV
    case 0x38: return mmio_r2138();  //OAMDATAREAD
    case 0x39: return mmio_r2139();  //VMDATALREAD
    case 0x3a: return mmio_r213a();  //VMDATAHREAD
    case 0x3b: return mmio_r213b();  //CGDATAREAD
    case 0x3c: return mmio_r213c();  //OPHCT
    case 0x3d: return mmio_r213d();  //OPVCT
    case 0x3e: return mmio_r213e();  //STAT77
    case 0x3f: return mmio_r213f();  //STAT78
  }

  return cpu.regs.mdr;
}

void PPU::mmio_write(unsigned addr, uint8 data) {
  cpu.synchronize_ppu();

  switch(addr & 0x3f) {
    case 0x00: return mmio_w2100(data);  //INIDISP
    case 0x01: return mmio_w2101(data);  //OBSEL
    case 0x02: return mmio_w2102(data);  //OAMADDL
    case 0x03: return mmio_w2103(data);  //OAMADDH
    case 0x04: return mmio_w2104(data);  //OAMDATA
    case 0x05: return mmio_w2105(data);  //BGMODE
    case 0x06: return mmio_w2106(data);  //MOSAIC
    case 0x07: return mmio_w2107(data);  //BG1SC
    case 0x08: return mmio_w2108(data);  //BG2SC
    case 0x09: return mmio_w2109(data);  //BG3SC
    case 0x0a: return mmio_w210a(data);  //BG4SC
    case 0x0b: return mmio_w210b(data);  //BG12NBA
    case 0x0c: return mmio_w210c(data);  //BG34NBA
    case 0x0d: return mmio_w210d(data);  //BG1HOFS
    case 0x0e: return mmio_w210e(data);  //BG1VOFS
    case 0x0f: return mmio_w210f(data);  //BG2HOFS
    case 0x10: return mmio_w2110(data);  //BG2VOFS
    case 0x11: return mmio_w2111(data);  //BG3HOFS
    case 0x12: return mmio_w2112(data);  //BG3VOFS
    case 0x13: return mmio_w2113(data);  //BG4HOFS
    case 0x14: return mmio_w2114(data);  //BG4VOFS
    case 0x15: return mmio_w2115(data);  //VMAIN
    case 0x16: return mmio_w2116(data);  //VMADDL
    case 0x17: return mmio_w2117(data);  //VMADDH
    case 0x18: return mmio_w2118(data);  //VMDATAL
    case 0x19: return mmio_w2119(data);  //VMDATAH
    case 0x1a: return mmio_w211a(data);  //M7SEL
    case 0x1b: return mmio_w211b(data);  //M7A
    case 0x1c: return mmio_w211c(data);  //M7B
    case 0x1d: return mmio_w211d(data);  //M7C
    case 0x1e: return mmio_w211e(data);  //M7D
    case 0x1f: return mmio_w211f(data);  //M7X
    case 0x20: return mmio_w2120(data);  //M7Y
    case 0x21: return mmio_w2121(data);  //CGADD
    case 0x22: return mmio_w2122(data);  //CGDATA
    case 0x23: return mmio_w2123(data);  //W12SEL
    case 0x24: return mmio_w2124(data);  //W34SEL
    case 0x25: return mmio_w2125(data);  //WOBJSEL
    case 0x26: return mmio_w2126(data);  //WH0
    case 0x27: return mmio_w2127(data);  //WH1
    case 0x28: return mmio_w2128(data);  //WH2
    case 0x29: return mmio_w2129(data);  //WH3
    case 0x2a: return mmio_w212a(data);  //WBGLOG
    case 0x2b: return mmio_w212b(data);  //WOBJLOG
    case 0x2c: return mmio_w212c(data);  //TM
    case 0x2d: return mmio_w212d(data);  //TS
    case 0x2e: return mmio_w212e(data);  //TMW
    case 0x2f: return mmio_w212f(data);  //TSW
    case 0x30: return mmio_w2130(data);  //CGWSEL
    case 0x31: return mmio_w2131(data);  //CGADDSUB
    case 0x32: return mmio_w2132(data);  //COLDATA
    case 0x33: return mmio_w2133(data);  //SETINI
  }
}

#endif
