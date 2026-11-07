#include <boost/ut.hpp>
#include <gnuradio-4.0/basic/SignalGenerator.hpp>

const boost::ut::suite liveFrequency = [] {
    using namespace boost::ut;
    "staged tone frequency changes the running clocked generator"_test = [] {
        gr::blocks::basic::SignalGenerator<float> generator({{"sample_rate", 1024.f}, {"tone_frequency", 128.f}});
        gr::PortOut<std::uint8_t> clock;
        gr::PortIn<float> output;
        expect(fatal(clock.connect(generator.clk_in).has_value()));
        expect(fatal(generator.out.connect(output).has_value()));
        generator.init(generator.progress);
        expect(fatal(generator.changeStateTo(gr::lifecycle::State::RUNNING).has_value()));
        expect(eq(generator.meta_information.value.at("tone_frequency::update_policy").value_or(std::string_view{}), std::string_view("live")));

        auto run = [&] {
            {
                auto ticks = clock.streamWriter().reserve<gr::SpanReleasePolicy::ProcessAll>(4);
                std::ranges::fill(ticks, std::uint8_t{0});
            }
            const auto result = generator.work(4);
            expect(result.status == gr::work::Status::OK);
            auto samples = output.streamReader().get(4);
            expect(fatal(eq(samples.size(), 4UZ)));
            std::vector<float> values(samples.begin(), samples.end());
            expect(samples.consume(4));
            return values;
        };
        const auto original = run();
        expect(approx(original[0], 0.f, 1e-5f));
        expect(approx(original[1], std::sqrt(0.5f), 1e-5f));
        expect(approx(original[2], 1.f, 1e-5f));

        expect(generator.settings().setStaged({{"tone_frequency", 256.f}}).empty());
        expect(eq(generator.tone_frequency.value, 128.f)); // The API stages; the work path applies.
        const auto updated = run();
        expect(eq(generator.tone_frequency.value, 256.f));
        expect(approx(updated[0], 0.f, 1e-5f));
        expect(approx(updated[1], -1.f, 1e-5f));
        expect(approx(updated[2], 0.f, 1e-5f));
        expect(approx(updated[3], 1.f, 1e-5f));
        expect(generator.changeStateTo(gr::lifecycle::State::REQUESTED_STOP).has_value());
    };
};

int main() {}
