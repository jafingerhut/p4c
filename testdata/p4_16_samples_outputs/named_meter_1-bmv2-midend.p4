#include <core.p4>
#define V1MODEL_VERSION 20180101
#include <v1model.p4>

struct meta_t {
    bit<32> meter_tag;
}

header ethernet_t {
    bit<48> dstAddr;
    bit<48> srcAddr;
    bit<16> etherType;
}

struct metadata {
    bit<32> _meta_meter_tag0;
}

struct headers {
    @name("ethernet")
    ethernet_t ethernet;
}

parser ParserImpl(packet_in packet, out headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    @name("ParserImpl.parse_ethernet") state parse_ethernet {
        packet.extract<ethernet_t>(hdr.ethernet);
        transition accept;
    }
    @name("ParserImpl.start") state start {
        transition parse_ethernet;
    }
}

control egress(inout headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    apply {
    }
}

@name("namedmeter") direct_meter<bit<32>>(MeterType.packets) my_meter;
control ingress(inout headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    @noWarn("unused") @name(".NoAction") action NoAction_1() {
    }
    @noWarn("unused") @name(".NoAction") action NoAction_2() {
    }
    @name("ingress._drop") action _drop() {
        mark_to_drop(standard_metadata);
    }
    @name("ingress._nop") action _nop() {
    }
    @name("ingress.m_filter") table m_filter_0 {
        actions = {
            _drop();
            _nop();
            NoAction_1();
        }
        key = {
            meta._meta_meter_tag0: exact @name("meta.meta.meter_tag");
        }
        size = 16;
        default_action = NoAction_1();
    }
    @name("ingress.m_action_2") action m_action_0(@name("meter_idx") bit<9> meter_idx) {
        standard_metadata.egress_spec = meter_idx;
        my_meter.read(meta._meta_meter_tag0);
    }
    @name("ingress._nop_2") action _nop_0() {
        my_meter.read(meta._meta_meter_tag0);
    }
    @name("ingress.m_table") table m_table_0 {
        actions = {
            m_action_0();
            _nop_0();
            NoAction_2();
        }
        key = {
            hdr.ethernet.srcAddr: exact @name("hdr.ethernet.srcAddr");
        }
        size = 16384;
        default_action = NoAction_2();
        meters = my_meter;
    }
    apply {
        m_table_0.apply();
        m_filter_0.apply();
    }
}

control DeparserImpl(packet_out packet, in headers hdr) {
    apply {
        packet.emit<ethernet_t>(hdr.ethernet);
    }
}

control verifyChecksum(inout headers hdr, inout metadata meta) {
    apply {
    }
}

control computeChecksum(inout headers hdr, inout metadata meta) {
    apply {
    }
}

V1Switch<headers, metadata>(ParserImpl(), verifyChecksum(), ingress(), egress(), computeChecksum(), DeparserImpl()) main;
