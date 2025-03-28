## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

import ns3waf

def configure(conf):
    ns3waf.check_modules(conf, ['core', 'internet', 'dce', 'point-to-point' ,'flow-monitor' ,'netanim'], mandatory = True)

def build(bld):
    bld.build_a_script('dce', needed = ['core', 'internet', 'dce', 'point-to-point' ,'flow-monitor' ,'netanim'],
	   		            target='bin/thesis',
			            # source=[ 'main.cc'],				            
			            source=[ 'thesis-ww.cc'],				            
			            # source=[ 'thesis-wlw.cc'],				            
			            # source=[ 'thesis-wlwl.cc'],				            
			            )

