#include <core.p4>
#include <psa.p4>

typedef bit<48> EthernetAddress;
header ethernet_t {
    EthernetAddress dstAddr;
    EthernetAddress srcAddr;
    bit<16>         etherType;
}

header output_data_t {
    bit<32> word0;
    bit<32> word1;
    bit<32> word2;
    bit<32> word3;
}

struct empty_metadata_t {
}

struct metadata_t {
}

struct headers_t {
    ethernet_t    ethernet;
    output_data_t output_data;
}

parser IngressParserImpl(packet_in pkt, out headers_t hdr, inout metadata_t user_meta, in psa_ingress_parser_input_metadata_t istd, in empty_metadata_t resubmit_meta, in empty_metadata_t recirculate_meta) {
    state start {
        pkt.extract<ethernet_t>(hdr.ethernet);
        pkt.extract<output_data_t>(hdr.output_data);
        transition accept;
    }
}

control cIngress(inout headers_t hdr, inout metadata_t user_meta, in psa_ingress_input_metadata_t istd, inout psa_ingress_output_metadata_t ostd) {
    @noWarnUnused @name(".multicast") action multicast() {
        ostd.drop = false;
        ostd.multicast_group = (MulticastGroupUint_t)hdr.ethernet.dstAddr;
    }
    @hidden table tbl_multicast {
        actions = {
            multicast();
        }
        const default_action = multicast();
    }
    apply {
        tbl_multicast.apply();
    }
}

parser EgressParserImpl(packet_in pkt, out headers_t hdr, inout metadata_t user_meta, in psa_egress_parser_input_metadata_t istd, in empty_metadata_t normal_meta, in empty_metadata_t clone_i2e_meta, in empty_metadata_t clone_e2e_meta) {
    state start {
        pkt.extract<ethernet_t>(hdr.ethernet);
        pkt.extract<output_data_t>(hdr.output_data);
        transition accept;
    }
}

control cEgress(inout headers_t hdr, inout metadata_t user_meta, in psa_egress_input_metadata_t istd, inout psa_egress_output_metadata_t ostd) {
    bit<32> ret;
    @name(".packet_path_to_int") action packet_path_to_int() {
        ret = (istd.packet_path == PSA_PacketPath_t.NORMAL ? 32w1 : (istd.packet_path == PSA_PacketPath_t.NORMAL_UNICAST ? 32w2 : (istd.packet_path == PSA_PacketPath_t.NORMAL_MULTICAST ? 32w3 : (istd.packet_path == PSA_PacketPath_t.CLONE_I2E ? 32w4 : (istd.packet_path == PSA_PacketPath_t.CLONE_E2E ? 32w5 : (istd.packet_path == PSA_PacketPath_t.RESUBMIT ? 32w6 : (istd.packet_path == PSA_PacketPath_t.RECIRCULATE ? 32w7 : ret)))))));
        hdr.output_data.word2 = ret;
    }
    @hidden action psamulticastbasic2bmv2l112() {
        hdr.output_data.word0 = (bit<32>)istd.egress_port;
        hdr.output_data.word1 = (bit<32>)(EgressInstanceUint_t)istd.instance;
    }
    @hidden table tbl_psamulticastbasic2bmv2l112 {
        actions = {
            psamulticastbasic2bmv2l112();
        }
        const default_action = psamulticastbasic2bmv2l112();
    }
    @hidden table tbl_packet_path_to_int {
        actions = {
            packet_path_to_int();
        }
        const default_action = packet_path_to_int();
    }
    apply {
        tbl_psamulticastbasic2bmv2l112.apply();
        tbl_packet_path_to_int.apply();
    }
}

control IngressDeparserImpl(packet_out buffer, out empty_metadata_t clone_i2e_meta, out empty_metadata_t resubmit_meta, out empty_metadata_t normal_meta, inout headers_t hdr, in metadata_t meta, in psa_ingress_output_metadata_t istd) {
    @hidden action psamulticastbasic2bmv2l122() {
        buffer.emit<ethernet_t>(hdr.ethernet);
        buffer.emit<output_data_t>(hdr.output_data);
    }
    @hidden table tbl_psamulticastbasic2bmv2l122 {
        actions = {
            psamulticastbasic2bmv2l122();
        }
        const default_action = psamulticastbasic2bmv2l122();
    }
    apply {
        tbl_psamulticastbasic2bmv2l122.apply();
    }
}

control EgressDeparserImpl(packet_out buffer, out empty_metadata_t clone_e2e_meta, out empty_metadata_t recirculate_meta, inout headers_t hdr, in metadata_t meta, in psa_egress_output_metadata_t istd, in psa_egress_deparser_input_metadata_t edstd) {
    @hidden action psamulticastbasic2bmv2l122_0() {
        buffer.emit<ethernet_t>(hdr.ethernet);
        buffer.emit<output_data_t>(hdr.output_data);
    }
    @hidden table tbl_psamulticastbasic2bmv2l122_0 {
        actions = {
            psamulticastbasic2bmv2l122_0();
        }
        const default_action = psamulticastbasic2bmv2l122_0();
    }
    apply {
        tbl_psamulticastbasic2bmv2l122_0.apply();
    }
}

IngressPipeline<headers_t, metadata_t, empty_metadata_t, empty_metadata_t, empty_metadata_t, empty_metadata_t>(IngressParserImpl(), cIngress(), IngressDeparserImpl()) ip;

EgressPipeline<headers_t, metadata_t, empty_metadata_t, empty_metadata_t, empty_metadata_t, empty_metadata_t>(EgressParserImpl(), cEgress(), EgressDeparserImpl()) ep;

PSA_Switch<headers_t, metadata_t, headers_t, metadata_t, empty_metadata_t, empty_metadata_t, empty_metadata_t, empty_metadata_t, empty_metadata_t>(ip, PacketReplicationEngine(), ep, BufferingQueueingEngine()) main;

