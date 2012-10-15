#!/bin/env python
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
    gROOT.Reset()
    gROOT.SetStyle("Plain")
    gStyle.SetOptStat(0)
    gStyle.SetPalette(1)
    gStyle.SetTitleStyle(0)
    gStyle.SetTitleBorderSize(0)
    gStyle.SetTitleOffset(1.1, "x")
    gStyle.SetTitleOffset(1.25, "y")
    gStyle.SetHistLineWidth(2)
    gStyle.SetLegendBorderSize(0)
    gROOT.ForceStyle()
    c1 = TCanvas("c1", "c1", 800, 600)
    

    fwater_2000 = TFile("../data/reduced/water-2000MeV.root")
    fwater_475 = TFile("../data/reduced/water-475MeV.root")
    fwater_210 = TFile("../data/reduced/water-210MeV.root")
    fwbls1_2000 = TFile("../data/reduced/wbls1-2000MeV.root")
    fwbls1_475 = TFile("../data/reduced/wbls1-475MeV.root")
    fwbls1_210 = TFile("../data/reduced/wbls1-210MeV.root")
    fwbls2_2000 = TFile("../data/reduced/wbls2-2000MeV.root")
    fwbls2_475 = TFile("../data/reduced/wbls2-475MeV.root")
    fwbls2_210 = TFile("../data/reduced/wbls2-210MeV.root")
    fls_2000 = TFile("../data/reduced/ls-2000MeV.root")
    fls_475 = TFile("../data/reduced/ls-475MeV.root")
    fls_210 = TFile("../data/reduced/ls-210MeV.root")
    
    hists = []
    legends = ['210 MeV', '475 MeV', '2 GeV']
    names = ['210', '475', '2000']

    # files = [fwater_210, fwater_475, fwater_2000]
    # saveTo = 'nPE_water.pdf'
    # material = 'Water'
    
    # files = [fwbls1_210, fwbls1_475, fwbls1_2000]
    # saveTo = 'nPE_wbls1.pdf'
    # material = 'WbLS 1%'
    
    # files = [fwbls2_210, fwbls2_475, fwbls2_2000]
    # saveTo = 'nPE_wbls2.pdf'
    # material = 'WbLS 4%'
    
    files = [fls_210, fls_475, fls_2000]
    saveTo = 'nPE_ls.pdf'
    material = 'LS'
    
    # legends = ['Water', 'WbLS 1%', 'WbLS 4%',]
    # files = [fwater_210, fwbls1_210, fwbls2_210]
    # saveTo = 'nPE_210.pdf'
    # material = '210 MeV'
    
    # legends = ['Water', 'WbLS 1%', 'WbLS 4%',]
    # files = [fwater_475, fwbls1_475, fwbls2_475]
    # saveTo = 'nPE_475.pdf'
    # material = '475 MeV'
    
    # legends = ['Water', 'WbLS 1%', 'WbLS 4%',]
    # files = [fwater_2000, fwbls1_2000, fwbls2_2000]
    # saveTo = 'nPE_2000.pdf'
    # material = '2 GeV'
    
    count = 0
    for f in files:
        t = f.Get("beamTree")
        histname = names[count]
        # t.Draw("nPE_T1>>nPE_"+histname+"(200, -5e4, 1e5)", "nPE_H1>2e4")
        t.Draw("nPE_T1>>nPE_"+histname+"(200, -5e5, 5e6)", "nPE_H1>2e4")
        hists.append(gDirectory.FindObject('nPE_'+histname))
        count += 1
    SetStyle(hists[0], title='Tub 1 (Teflon), '+material, xTitle='Charge')
    SetStyle(hists[1], lineColor=ROOT.kRed)
    SetStyle(hists[2], lineColor=ROOT.kBlue)
    hists[0].Draw()
    hists[1].Draw("same")    
    hists[2].Draw("same")
    leg = TLegend(0.65,0.87-len(hists)*0.07,0.85,0.87)
    for i in range(len(hists)):
        leg.AddEntry(hists[i], '  ' + legends[i], 'l')
        print '%s: %d | ' % (legends[i], hists[i].GetEntries()),
    print
    leg.SetFillColor(ROOT.kWhite)
    leg.Draw()   
    c1.SaveAs("pdf/"+saveTo)
    