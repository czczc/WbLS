#!/usr/bin/env python
import os, glob, sys

import ROOT
from ROOT import gROOT, gStyle, gDirectory, gPad
from ROOT import TCanvas, TLegend, TLatex, TFile
from ROOT import TChain, TH1F
import math

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
    
if __name__ == '__main__':
    figType = 'pdf'
    
    c1 = TCanvas("c1", "c1", 800, 600)
    
    t = TChain("/Event/Sim")
    t.Add('data/tree*.root')
    
    # # hit_photon_wavelength
    # histname = 'hit_photon_wavelength'
    # drawStr = "hit_wl"
    # drawRange = "(300, 300, 600)"
    # 
    # t.Draw(drawStr+'>>'+histname+drawRange)
    # h = gDirectory.FindObject(histname)
    # h.Draw()
    # h.SetTitle("")
    # h.GetXaxis().SetTitle("Hit Photon Wavelength [nm]")
    # c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))
    
    # decay_mode
    histname = 'decay_mode'
    drawStr = "mode"
    drawRange = ""
    
    t.Draw(drawStr+'>>'+histname+drawRange)
    h = gDirectory.FindObject(histname)
    h.Scale(1./h.Integral())
    # h.GetXaxis().SetBinLabel(11, "#mu^{+} + #nu_{#mu}");
    # h.GetXaxis().SetBinLabel(21, "#pi^{+} + #pi^{0}");
    # h.LabelsOption("h", "X")
    h.Draw()
    l1 = TLatex(15, 0.6, "#mu^{+} + #nu_{#mu} (63.47%)"); l1.Draw()
    l2 = TLatex(25, 0.2, "#pi^{+} + #pi^{0} (21.13%)"); l2.Draw()
    h.SetTitle("")
    h.GetXaxis().SetTitle("K^{+} Decay Mode")
    h.GetYaxis().SetTitle("Branching Ratio")
    c1.SaveAs(('./'+figType+'/'+histname+'.'+figType))
    
    
    
    