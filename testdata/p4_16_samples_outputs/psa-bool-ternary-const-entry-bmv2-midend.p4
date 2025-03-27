#include <core.p4>
#include <bmv2/psa.p4>

header ethernet_t {
    bit<48> dstAddr;
    bit<48> srcAddr;
    bit<16> etherType;
}

struct fwd_metadata_t {
}

struct empty_t {
}

struct metadata {
}

struct headers {
    ethernet_t ethernet;
}

parser IngressParserImpl(packet_in buffer, out headers parsed_hdr, inout metadata user_meta, in psa_ingress_parser_input_metadata_t istd, in empty_t resubmit_meta, in empty_t recirculate_meta) {
    state start {
        parsed_hdr.ethernet.setInvalid();
        buffer.extract<ethernet_t>(parsed_hdr.ethernet);
        transition accept;
    }
}

parser EgressParserImpl(packet_in buffer, out headers parsed_hdr, inout metadata user_meta, in psa_egress_parser_input_metadata_t istd, in empty_t normal_meta, in empty_t clone_i2e_meta, in empty_t clone_e2e_meta) {
    state start {
        parsed_hdr.ethernet.setInvalid();
        buffer.extract<ethernet_t>(parsed_hdr.ethernet);
        transition accept;
    }
}

control ingress(inout headers hdr, inout metadata user_meta, in psa_ingress_input_metadata_t istd, inout psa_ingress_output_metadata_t ostd) {
    @noWarn("unused") @name(".NoAction") action NoAction_1() {
    }
    @name("ingress.DummyAction") action DummyAction() {
    }
    @name("ingress.test_table") table test_table_0 {
        key = {
            hdr.ethernet.isValid(): ternary @name("hdr.ethernet.$valid$");
        }
        actions = {
            DummyAction();
            NoAction_1();
        }
        const entries = {
                        true : DummyAction();
        }
        default_action = NoAction_1();
    }
    apply {
        test_table_0.apply();
    }
}

control egress(inout headers hdr, inout metadata user_meta, in psa_egress_input_metadata_t istd, inout psa_egress_output_metadata_t ostd) {
    apply {
    }
}

control IngressDeparserImpl(packet_out buffer, out empty_t clone_i2e_meta, out empty_t resubmit_meta, out empty_t normal_meta, inout headers hdr, in metadata meta, in psa_ingress_output_metadata_t istd) {
    @hidden action psaboolternaryconstentrybmv2l119() {
        buffer.emit<ethernet_t>(hdr.ethernet);
    }
    @hidden table tbl_psaboolternaryconstentrybmv2l119 {
        actions = {
            psaboolternaryconstentrybmv2l119();
        }
        const default_action = psaboolternaryconstentrybmv2l119();
    }
    apply {
        tbl_psaboolternaryconstentrybmv2l119.apply();
    }
}

control EgressDeparserImpl(packet_out buffer, out empty_t clone_e2e_meta, out empty_t recirculate_meta, inout headers hdr, in metadata meta, in psa_egress_output_metadata_t istd, in psa_egress_deparser_input_metadata_t edstd) {
    @hidden action psaboolternaryconstentrybmv2l119_0() {
        buffer.emit<ethernet_t>(hdr.ethernet);
    }
    @hidden table tbl_psaboolternaryconstentrybmv2l119_0 {
        actions = {
            psaboolternaryconstentrybmv2l119_0();
        }
        const default_action = psaboolternaryconstentrybmv2l119_0();
    }
    apply {
        tbl_psaboolternaryconstentrybmv2l119_0.apply();
    }
}

IngressPipeline<headers, metadata, empty_t, empty_t, empty_t, empty_t>(IngressParserImpl(), ingress(), IngressDeparserImpl()) ip;
EgressPipeline<headers, metadata, empty_t, empty_t, empty_t, empty_t>(EgressParserImpl(), egress(), EgressDeparserImpl()) ep;
PSA_Switch<headers, metadata, headers, metadata, empty_t, empty_t, empty_t, empty_t, empty_t>(ip, PacketReplicationEngine(), ep, BufferingQueueingEngine()) main;
