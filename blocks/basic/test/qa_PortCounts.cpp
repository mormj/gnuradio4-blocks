#include <boost/ut.hpp>
#include <gnuradio-4.0/basic/Selector.hpp>
#include <gnuradio-4.0/basic/SyncBlock.hpp>
#include <gnuradio-4.0/math/Math.hpp>

const boost::ut::suite portCounts = [] {
    using namespace boost::ut;
    "declared counts agree with default and accepted block configurations"_test = [] {
        gr::blocks::basic::Selector<float> selector;
        selector.settings().init();
        expect(selector.inputs.empty() && selector.outputs.empty());
        expect(selector.meta_information.value.contains("n_inputs::port_count_for"));
        std::ignore = selector.settings().setStaged({{"n_inputs", 2U}, {"n_outputs", 3U}});
        std::ignore = selector.settings().applyStagedParameters();
        expect(eq(selector.inputs.size(), 2UZ));
        expect(eq(selector.outputs.size(), 3UZ));

        gr::blocks::basic::SyncBlock<float> sync;
        sync.settings().init();
        expect(sync.meta_information.value.contains("n_ports::port_count_for"));
        std::ignore = sync.settings().setStaged({{"n_ports", 2U}});
        std::ignore = sync.settings().applyStagedParameters();
        expect(eq(sync.inputs.size(), 2UZ));
        expect(eq(sync.outputs.size(), 2UZ));

        gr::blocks::math::Add<float> add;
        add.settings().init();
        expect(add.meta_information.value.contains("n_inputs::port_count_for"));
        std::ignore = add.settings().setStaged({{"n_inputs", 3U}});
        std::ignore = add.settings().applyStagedParameters();
        expect(eq(add.in.size(), 3UZ));
    };
};

int main() {}
