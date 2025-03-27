#include <core.p4>
#include <v1model.p4>

typedef standard_metadata_t std_meta_t;

header standard_t<T> {
  T src;
  T dst;
}

struct headers_t<T> {
  standard_t<T> standard;
}

struct meta_t { }

parser MyParser(packet_in pkt, out headers_t<bit<8>> hdr, inout meta_t meta, inout std_meta_t std_meta) {
    state start {
        pkt.extract(hdr.standard);
        transition accept;
    }
}

control MyVerifyChecksum(inout headers_t<bit<8>> hdr, inout meta_t meta) {
    apply { }
}
control MyComputeChecksum(inout headers_t<bit<8>> hdr, inout meta_t meta) {
    apply { }
}
control MyIngress(inout headers_t<bit<8>> hdr, inout meta_t meta, inout std_meta_t std_meta) {
   apply { }
}
control MyEgress(inout headers_t<bit<8>> hdr, inout meta_t meta, inout std_meta_t std_meta) {
    apply { }
}
control MyDeparser(packet_out pkt, in headers_t<bit<8>> hdr) {
    apply { }
}
V1Switch(MyParser(), MyVerifyChecksum(), MyIngress(), MyEgress(), MyComputeChecksum(), MyDeparser()) main;
