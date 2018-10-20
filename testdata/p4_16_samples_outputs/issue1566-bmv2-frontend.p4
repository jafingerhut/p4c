#include <core.p4>
#include <v1model.p4>

typedef bit<48> EthernetAddress;
header Ethernet_h {
    EthernetAddress dstAddr;
    EthernetAddress srcAddr;
    bit<16>         etherType;
}

struct Parsed_packet {
    Ethernet_h ethernet;
}

struct metadata_t {
    bit<4> a;
    bit<4> b;
}

parser parserI(packet_in pkt, out Parsed_packet hdr, inout metadata_t meta, inout standard_metadata_t stdmeta) {
    state start {
        pkt.extract<Ethernet_h>(hdr.ethernet);
        transition accept;
    }
}

control cIngress(inout Parsed_packet hdr, inout metadata_t meta, inout standard_metadata_t stdmeta) {
    bit<16> x_0;
    @name("cIngress.E.c1.stats") counter(32w65536, CounterType.packets) E_c1_stats;
    apply {
        hdr.ethernet.etherType = hdr.ethernet.etherType << 1;
        x_0 = hdr.ethernet.etherType;
        x_0 = x_0 + 16w1;
        E_c1_stats.count((bit<32>)x_0);
        hdr.ethernet.etherType = x_0;
        hdr.ethernet.etherType = hdr.ethernet.etherType << 3;
        x_0 = hdr.ethernet.etherType;
        x_0 = x_0 + 16w1;
        E_c1_stats.count((bit<32>)x_0);
        hdr.ethernet.etherType = x_0;
    }
}

control cEgress(inout Parsed_packet hdr, inout metadata_t meta, inout standard_metadata_t stdmeta) {
    apply {
    }
}

control DeparserI(packet_out packet, in Parsed_packet hdr) {
    apply {
        packet.emit<Ethernet_h>(hdr.ethernet);
    }
}

control vc(inout Parsed_packet hdr, inout metadata_t meta) {
    apply {
    }
}

control uc(inout Parsed_packet hdr, inout metadata_t meta) {
    apply {
    }
}

V1Switch<Parsed_packet, metadata_t>(parserI(), vc(), cIngress(), cEgress(), uc(), DeparserI()) main;

