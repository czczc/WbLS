#!/usr/bin/env python
import os, glob, sys

import ROOT
from ROOT import gROOT, gStyle, gDirectory, gPad
from ROOT import TCanvas, TLegend, TLatex, TFile
from ROOT import TChain, TH1F, TF1, TH2F, TGraph
import math
from array import array

def DefaultStyle():
    gROOT.Reset()
    gROOT.SetStyle("Plain")
    gStyle.SetOptStat(0)
    gStyle.SetOptFit(111)
    gStyle.SetPalette(1)
    gStyle.SetTitleStyle(0)
    gStyle.SetTitleBorderSize(0)
    gStyle.SetTitleOffset(1.1, "x")
    gStyle.SetTitleOffset(1.25, "y")
    gStyle.SetHistLineWidth(2)
    gStyle.SetLegendBorderSize(0)
    gStyle.SetMarkerStyle(20)
    gStyle.SetMarkerSize(0.7)
    gROOT.ForceStyle()


def SetStyle(h, *seq, **kwargs):
    'Set the Paiter Style'
    
    title = kwargs.get('title', '')
    xTitle = kwargs.get('xTitle', '')
    yTitle = kwargs.get('yTitle', '')
    lineColor = kwargs.get('lineColor', '')
    markerColor = kwargs.get('markerColor', '')
    markerSize = kwargs.get('markerSize', '')
    markerStyle = kwargs.get('markerStyle', '')
    
    h.SetLineWidth(2)
    if title: h.SetTitle(title)
    if xTitle: h.SetXTitle(xTitle)
    if yTitle: h.SetYTitle(yTitle)
    if lineColor: h.SetLineColor(lineColor)
    if markerColor: h.SetMarkerColor(markerColor)
    if markerSize: h.SetMarkerSize(markerSize)
    if markerStyle: h.SetMarkerStyle(markerStyle)


def Michel(c1, t):
    f1 = TF1("f","exp(-x/2200)", 0, 1e6)
    print "Michel Positron >100ns Eff:", (1-f1.Integral(0,100)/f1.Integral(0, 1e6))*100, "%"
    print "Michel Positron <10ns Eff:", f1.Integral(0,10)/f1.Integral(0, 1e6)*100, "%"
    
    # muon decay time (Michel electron)
    histname = 'ch1_michele_t'
    drawStr = "ch1_michele_t/1000"
    drawRange = "(100, 0, 10)"
    drawCuts = "mode == 11"
    
    t.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    h = gDirectory.FindObject(histname)
    h.SetTitle("#mu^{+} Decay Time")
    h.GetXaxis().SetTitle("Michel Positron Time [#mus]")
    fit = TF1("fit", "[0]*exp(-x/[1])", 0, 10);
    fit.SetParNames("Constant", "Life Time")
    fit.SetParameter(1, 1) # initialize to 1 us
    h.Fit(fit, "QR")
    h.Draw("e")
    c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))
    
    # Michel positron energy
    histname = 'ch1_michele_e'
    drawStr = "ch1_michele_e"
    drawRange = "(60, 0, 60)"
    drawCuts = "mode == 11"
    
    t.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    h = gDirectory.FindObject(histname)
    h.SetTitle("Michel e^{+} Energy")
    h.GetXaxis().SetTitle("Energy [MeV]")
    h.Draw()
    e_cut = 6  # MeV
    cut_bin = h.FindBin(6)
    print "Michele Positron > 6 MeV eff:", h.Integral(cut_bin, 60)/h.Integral()
    c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))
    
    
def CH1_MuonE(c1, t):
    'muon energy'
    histname = 'ch1_muon_e'
    drawStr = "ch1_muon_e"
    drawRange = "(100, 0, 400)"
    drawCuts = "mode == 11"
    
    t.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    h = gDirectory.FindObject(histname)
    h.SetTitle("#mu^{+} Energy")
    h.GetXaxis().SetTitle("Energy [MeV]")
    h.Draw()
    gPad.SetLogy()
    
    trueE_bin = h.FindBin(152)
    print "K+ decay on flight:", float(1-h.Integral(trueE_bin-1, trueE_bin+1)/h.Integral())*100, "%"
    
    c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))


def CH1_NPE(c1, t):
    """NPE distribution for channel 1"""
    # Kaon PE (tc < 25ns, kaon_decay_t > 12ns)
    histname = 'ch1_kaon_npe'
    drawStr = "ch1_kaon_npe"
    drawRange = "(50, 100, 300)"
    drawCuts = "mode == 11 && ch1_michele_t>100"
    
    t.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    h = gDirectory.FindObject(histname)
    h.SetTitle("Kaon Deposited Energy")
    h.GetXaxis().SetTitle("Number of Photoelectrons")
    fit = TF1("fit", "gaus", 100, 300)
    fit.SetLineColor(ROOT.kRed)
    h.Fit(fit, "QR")
    h.Draw("e")
    c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))    
    
    # Muon PE (kaon_decay_t-1 < tc < 100 ns, kaon_decay_t > 10ns )
    histname = 'ch1_muon_npe'
    drawStr = "ch1_muon_npe"
    drawRange = "(75, 600, 900)"
    drawCuts = "mode == 11 && ch1_michele_t>100"
    
    t.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    h = gDirectory.FindObject(histname)
    h.SetTitle("Muon Deposited Energy")
    h.GetXaxis().SetTitle("Number of Photoelectrons")
    fit = TF1("fit", "gaus", 600, 900)
    fit.SetLineColor(ROOT.kRed)
    h.Fit(fit, "QR")
    h.Draw("e")
    c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))     
    
    # Promt PE (Kaon + Muon, tc < 100 ns)
    histname = 'ch1_prompt_npe'
    drawStr = "prompt_npe"
    drawRange = "(400, 0, 1600)"
    drawCuts = "mode == 11 && ch1_michele_t>100"
    
    t.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    h = gDirectory.FindObject(histname)
    min_bin = h.FindBin(800)
    max_bin = h.FindBin(1100)
    print "Prompt Energy Cut Eff:", float(h.Integral(min_bin, max_bin))/h.Integral()*100, "%"
    
    h.SetTitle("Total Prompt Energy (< 100 ns)")
    h.GetXaxis().SetTitle("Number of Photoelectrons")
    h.GetXaxis().SetRangeUser(700, 1200)
    fit = TF1("fit", "gaus", 700, 1200)
    fit.SetLineColor(ROOT.kRed)
    h.Fit(fit, "QR")
    h.Draw("e")
    
    c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))


def BG1_NPE(c1, t):
    histname  = 'bg1_promptNPE_vs_muonE'
    drawStr   = "prompt_npe:bg1_muon_e"
    drawRange = ""
    drawCuts  = "mode == -11 && bg1_michele_t>100"
    
    t.Draw(drawStr+'>>'+histname+drawRange, drawCuts, "prof")
    h = gDirectory.FindObject(histname)
    h.SetTitle("Muon Light Yield")
    h.GetXaxis().SetTitle("Muon Energy")
    h.GetYaxis().SetTitle("Number of Photoelectrons")
    h.GetYaxis().SetRangeUser(550, 1400)
    h.SetMarkerStyle(0)
    h.Draw()
    gPad.SetGridx(); gPad.SetGridy()
    c1.SaveAs('./'+figType+'/'+histname+'.'+figType)


def RisingT(c1, t, t_bg):
    'Rising time (Visible)'
    t_cut = 10  # ns
    
    histname = 'ch1_rising_dt_vis'
    drawStr = "rising_dt_vis"
    drawRange = "(100, 0, 100)"
    drawCuts = "mode == 11 && ch1_michele_t>100 && prompt_npe>800 && prompt_npe<1100"
    
    t.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    h = gDirectory.FindObject(histname)
    h.SetTitle("Rising Time of The Waveform")
    h.GetXaxis().SetTitle("#DeltaT_{15% - 85%} [ns]")
    h.SetLineColor(ROOT.kRed)
    # h.SetFillColor(ROOT.kCyan)
    gPad.SetLogy()
    h.Draw()
    print "SIG efficiency:", h.Integral(t_cut,100)/h.Integral()
    c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))
    
    # BG 1 Rising time (visible)
    histname = 'bg1_rising_dt_vis'
    drawStr = "rising_dt_vis"
    drawRange = "(100, 0, 100)"
    drawCuts = "mode == -11 && bg1_michele_t>100 && prompt_npe>800 && prompt_npe<1100"
    
    t_bg.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    h_bg = gDirectory.FindObject(histname)
    h_bg.SetTitle("Rising Time of The Waveform")
    h_bg.GetXaxis().SetTitle("#DeltaT_{15% - 85%} [ns]")
    # h_bg.SetFillColor(ROOT.kMagenta)
    h_bg.Draw()
    print "BG acceptance:", h_bg.Integral(t_cut,100)/h.Integral()
    c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))
    
    h.Scale(2.5)
    h.Draw()
    h_bg.Draw("same")
    c1.SaveAs('./'+figType+'/ch1bg1_risingT.'+figType)


def ExcessE(c1, t, t_bg):
    # Excess Energy
    e_cut = 150  # PE
    
    histname = 'ch1_kaon_npe_recon'
    drawStr = "prompt_npe-ch1_muon_npe_recon"
    drawRange = "(100, -150, 350)"
    drawCuts = "mode == 11 && ch1_michele_t>100 && prompt_npe>800 && prompt_npe<1100"
    
    t.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    h = gDirectory.FindObject(histname)
    h.SetTitle("Excess Deposited Energy")
    h.GetXaxis().SetTitle("Number of Photoelectrons")
    h.SetLineColor(ROOT.kRed)
    h.Draw()
    cut = h.FindBin(e_cut)
    print "SIG Efficiency: ", h.Integral(cut, 100)/h.Integral()
    c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))
    
    histname = 'ch1_kaon_npe_recon_w_inv_risingTcut'
    drawCuts = "mode == 11 && ch1_michele_t>100 && prompt_npe>800 && prompt_npe<1100 && rising_dt_vis<10"
    
    t.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    h2 = gDirectory.FindObject(histname)
    cut = h2.FindBin(e_cut)
    print "SIG Efficiency with Inverse Rising Time cut: ", h2.Integral(cut, 100)/h2.Integral()
    
    # BG
    histname = 'bg1_kaon_npe_recon'
    drawStr = "prompt_npe-bg1_muon_npe_recon"
    drawRange = "(100, -150, 350)"
    drawCuts = "mode == -11 && bg1_michele_t>100 && prompt_npe>800 && prompt_npe<1100"
    
    t_bg.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    h_bg = gDirectory.FindObject(histname)
    h_bg.SetTitle("Excess Deposited Energy")
    h_bg.GetXaxis().SetTitle("Number of Photoelectrons")
    h_bg.Draw()
    cut = h_bg.FindBin(e_cut)
    print "BG acceptance: ", h_bg.Integral(cut, 100)/h_bg.Integral()
    c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))
    
    h.Draw()
    h_bg.Draw("same")
    c1.SaveAs('./'+figType+'/ch1bg1_excessE.'+figType)
    


def RisingT_ExcessE(c1, t, t_bg):
    # 2D: rising time vs excess energy
    histname  = 'ch1_risingT_vs_excessE'
    drawStr   = "rising_dt_vis:prompt_npe-ch1_muon_npe_recon"
    drawRange = "(100, -150, 350, 100, 0, 100)"
    drawCuts  = "mode == 11 && ch1_michele_t>100 && prompt_npe>800 && prompt_npe<1100"
    
    t.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    h = gDirectory.FindObject(histname)
    h.SetTitle("")
    h.GetXaxis().SetTitle("Number of Excess Photoelectrons")
    h.GetYaxis().SetTitle("Rising Time [ns]")
    h.SetMarkerColor(ROOT.kRed)
    h.SetMarkerSize(0.3)
    h.Draw("")
    c1.SaveAs('./'+figType+'/'+histname+'.'+figType)
    
    # == BG 1 ===
    histname  = 'bg1_risingT_vs_excessE'
    drawStr   = "rising_dt_vis:prompt_npe-bg1_muon_npe_recon"
    drawRange = "(100, -150, 350, 100, 0, 100)"
    drawCuts  = "mode == -11 && bg1_michele_t>100 && prompt_npe>800 && prompt_npe<1100"
    
    t_bg.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    h_bg = gDirectory.FindObject(histname)
    h_bg.SetTitle("")
    h_bg.GetXaxis().SetTitle("Number of Excess Photoelectrons")
    h_bg.GetYaxis().SetTitle("Rising Time [ns]")
    h_bg.SetMarkerSize(0.3)
    h_bg.Draw("")
    c1.SaveAs('./'+figType+'/'+histname+'.'+figType)
    
    h.GetYaxis().SetRangeUser(0, 80)
    h.GetXaxis().SetRangeUser(-100, 350)
    h.Draw()
    h_bg.Draw("same")
    c1.SaveAs('./'+figType+'/ch1bg1_risingT_vs_excessE.'+figType)


if __name__ == '__main__':
    '''Plot Various Distributions'''
    
    DefaultStyle()
    
    figType = 'pdf'
    
    c1 = TCanvas("c1", "c1", 800, 600)
    
    t = TChain("/Event/Stats")
    stats_dir = '../tree/kaon_105MeV_py90'
    t.Add(stats_dir+'/*.root')
    
    t_bg1 = TChain("/Event/Stats")
    stats_dir_bg1 = '../stats/muon_140-240MeV_py90'
    t_bg1.Add(stats_dir_bg1+'/*.root')
    
    Michel(c1, t)
    # CH1_MuonE(c1, t)
    # CH1_NPE(c1, t)
    
    # BG1_NPE(c1, t_bg1)
    # RisingT(c1, t, t_bg1)
    # ExcessE(c1, t, t_bg1)
    # RisingT_ExcessE(c1, t, t_bg1)
    
    
    # histname = "mu_opening_angle"
    # rindex = 1.333*1.0112
    # m = 105.658  # MeV
    # x = array('f', [])
    # y = array('f', [])
    # for i in range(0, 250):
    #     KE = 52 + i
    #     cos = 1/rindex/math.sqrt(1-m*m/(KE+m)/(KE+m))
    #     angle = math.acos(cos)*180/3.1415
    #     x.append(KE)
    #     y.append(angle)
    # g = TGraph(len(x), x, y)
    # g.SetLineWidth(2)
    # g.SetTitle("Muon Cherenkov Opening Angle")
    # g.GetXaxis().SetTitle("Kinetic Energy [MeV]")
    # g.GetYaxis().SetTitle("Angle [degree]")
    # g.Draw("AL")
    # gPad.SetGridx()
    # gPad.SetGridy()
    # c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))
    
    # angle = 0
    # print "angle: ", angle, "| cos(angle):", cos
    
    # # kaon decay time
    # histname = 'kaon_decay_t'
    # drawStr = "kaon_decay_t"
    # drawRange = "(60, 0, 60)"
    # 
    # t.Draw(drawStr+'>>'+histname+drawRange)
    # h = gDirectory.FindObject(histname)
    # h.SetTitle("K^{+} Decay Time")
    # h.GetXaxis().SetTitle("Decay Time [ns]")
    # fit = TF1("fit", "[0]*exp(-x/[1])", 0, 60);
    # fit.SetParNames("Constant", "Life Time")
    # fit.SetParameter(1, 10) # initialize to 10 ns
    # h.Fit(fit, "QR")
    # h.Draw("e")
    # c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))
    
    
    # ============== Channel One ==================
    
    
    

    

    
    # # Rising time
    # histname = 'ch1_rising_dt'
    # drawStr = "rising_dt"
    # drawRange = "(100, 0, 100)"
    # drawCuts = "mode == 11"
    # 
    # t.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    # h = gDirectory.FindObject(histname)
    # h.SetTitle("Rising Time of The Waveform")
    # h.GetXaxis().SetTitle("#DeltaT_{15% - 85%} [ns]")
    # h.SetFillColor(ROOT.kCyan)
    # gPad.SetLogy()
    # h.Draw("")
    # print "efficiency at 7ns cut:", h.Integral(7,100) / h.Integral()
    # c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))    


    
    # # Early charge ratio
    # histname = 'ch1_early_charge_ratio'
    # drawStr = "early_charge_ratio"
    # drawRange = "(100, 0, 1)"
    # drawCuts = "mode == 11"
    # 
    # t.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    # h = gDirectory.FindObject(histname)
    # h.SetTitle("")
    # h.GetXaxis().SetTitle("Early Charge Ratio")
    # h.Draw("")
    # c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))
    

    
    # # 2D: rising time vs excess energy
    # histname = 'ch1_risingT_vs_excessE'
    # drawStr = "rising_dt_vis:prompt_npe-ch1_muon_npe_recon"
    # drawRange = "(100, -150, 350, 100, 0, 100)"
    # drawCuts = "mode == 11 && prompt_npe>800 && prompt_npe<1100"
    # 
    # t.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    # h = gDirectory.FindObject(histname)
    # h.SetTitle("Excess Deposited Energy")
    # h.GetXaxis().SetTitle("Number of Excess Photoelectrons")
    # h.GetYaxis().SetTitle("Rising Time [ns]")
    # h.Draw("")
    # c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))
    
    # === BG 1 ====
    # # BG 1 Rising time
    # histname = 'bg1_rising_dt'
    # drawStr = "rising_dt"
    # drawRange = "(10, 0, 10)"
    # drawCuts = "mode == -11"
    # 
    # t.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    # h = gDirectory.FindObject(histname)
    # h.SetTitle("Rising Time of The Waveform")
    # h.GetXaxis().SetTitle("#DeltaT_{15% - 85%} [ns]")
    # h.SetFillColor(ROOT.kCyan)
    # gPad.SetLogy()
    # h.Draw("")
    # print "efficiency at 6ns cut:", h.Integral(6,10) / h.Integral()
    # c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))
    
    

    
    # # Promt PE (Muon, tc < 100 ns)
    # histname = 'bg1_prompt_npe'
    # drawStr = "prompt_npe"
    # drawRange = "(100, 500, 1500)"
    # drawCuts = "mode == -11"
    # 
    # t.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    # h = gDirectory.FindObject(histname)
    # h.SetTitle("Total Prompt Energy (time < 100 ns)")
    # h.GetXaxis().SetTitle("Number of Photoelectrons")
    # h.Draw("e")
    # c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))     
    
    # # Early charge ratio
    # histname = 'bg1_early_charge_ratio'
    # drawStr = "early_charge_ratio"
    # drawRange = "(100, 0, 1)"
    # drawCuts = "mode == -11"
    # 
    # t.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    # h = gDirectory.FindObject(histname)
    # h.SetTitle("")
    # h.GetXaxis().SetTitle("Early Charge Ratio")
    # h.Draw("")
    # c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))
    

    
    # # 2D: rising time vs excess energy
    # histname = 'bg1_risingT_vs_excessE'
    # drawStr = "rising_dt_vis:prompt_npe-bg1_muon_npe_recon"
    # drawRange = "(100, -150, 350, 100, 0, 100)"
    # drawCuts = "mode == -11 && prompt_npe>800 && prompt_npe<1100"
    # 
    # t.Draw(drawStr+'>>'+histname+drawRange, drawCuts)
    # h = gDirectory.FindObject(histname)
    # h.SetTitle("Excess Deposited Energy")
    # h.GetXaxis().SetTitle("Number of Excess Photoelectrons")
    # h.GetYaxis().SetTitle("Rising Time [ns]")
    # h.Draw("")
    # c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))
    
    
    