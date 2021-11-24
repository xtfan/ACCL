/*******************************************************************************
#  Copyright (C) 2021 Xilinx, Inc
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
# *******************************************************************************/

#include "vnx.h"

using namespace hls;
using namespace std;

void vnx_depacketizer(	stream<ap_axiu<DATA_WIDTH,0,0,16> > & in,
			stream<ap_axiu<DATA_WIDTH,0,0,16> > & out,
			stream<ap_axiu<4*32,0,0,0> > & sts) {
#pragma HLS INTERFACE axis register both port=in
#pragma HLS INTERFACE axis register both port=out
#pragma HLS INTERFACE axis register both port=sts
#pragma HLS INTERFACE s_axilite port=return

unsigned const bytes_per_word = DATA_WIDTH/8;

//copy count from header into sts stream
ap_axiu<DATA_WIDTH,0,0,16> inword = in.read();
ap_axiu<DATA_WIDTH,0,0,16> outword;
ap_axiu<4*32,0,0,0> stsword;
//read header and put in sts stream
int count 	= (inword.data)(HEADER_COUNT_END, HEADER_COUNT_START);
int strm 	= (inword.data)(HEADER_STRM_END	, HEADER_STRM_START	);

if(strm == 0){
	stsword.data = inword.data(4*32-1,0);
	stsword.last = 1;
	stsword.keep = -1;
	sts.write(stsword);
}

while(count > 0){
#pragma HLS PIPELINE II=1
	inword = in.read();
	outword.data = inword.data;
	outword.keep = inword.keep;
	outword.dest = strm;
	count -= bytes_per_word;
	if(count <= 0){
		outword.last = 1;
	}else{
		outword.last = 0;
	}
	out.write(outword);
}

}
