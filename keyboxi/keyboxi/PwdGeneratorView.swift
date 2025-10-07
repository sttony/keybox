import SwiftUI
import Foundation

struct PwdGeneratorView: View {
    // Wrapper to C++ generator
    private let gen: OPasswordGenerator

    // State mirrors generator options
    @State private var needLower: Bool
    @State private var needUpper: Bool
    @State private var needDigit: Bool
    @State private var needMinus: Bool
    @State private var needAdd: Bool
    @State private var needShift18: Bool
    @State private var needBrace: Bool
    @State private var needSpace: Bool
    @State private var needQuestion: Bool
    @State private var needSlash: Bool
    @State private var needGreaterLess: Bool
    @State private var length: Int

    @State private var password: String = ""
    @State private var showPassword: Bool = false

    var onUse: (String) -> Void
    var onCancel: () -> Void

    init(onUse: @escaping (String) -> Void = { _ in }, onCancel: @escaping () -> Void = {}) {
        self.onUse = onUse
        self.onCancel = onCancel
        // Construct generator using global g_RG behind the wrapper
        let g = OPasswordGenerator(random: ORandomGenerator(NSNumber(value: kSalsa20Type).uint32Value))
        self.gen = g
        // Seed initial state from generator defaults
        _needLower = State(initialValue: g.getLower())
        _needUpper = State(initialValue: g.getUpper())
        _needDigit = State(initialValue: g.getDigit())
        _needMinus = State(initialValue: g.getMinus())
        _needAdd = State(initialValue: g.getAdd())
        _needShift18 = State(initialValue: g.getShift1_8())
        _needBrace = State(initialValue: g.getBrace())
        _needSpace = State(initialValue: g.getSpace())
        _needQuestion = State(initialValue: g.getQuestion())
        _needSlash = State(initialValue: g.getSlash())
        _needGreaterLess = State(initialValue: g.getGreaterLess())
        _length = State(initialValue: Int(g.getLength()))
    }

    var body: some View {
        VStack(spacing: 0) {
            Form {
                Section(header: Text("Characters")) {
                    Toggle("Lowercase (a-z)", isOn: bindingBool($needLower, setter: gen.setLower))
                    Toggle("Uppercase (A-Z)", isOn: bindingBool($needUpper, setter: gen.setUpper))
                    Toggle("Digits (0-9)", isOn: bindingBool($needDigit, setter: gen.setDigit))
                    Toggle("Minus (-)", isOn: bindingBool($needMinus, setter: gen.setMinus))
                    Toggle("Add (+)", isOn: bindingBool($needAdd, setter: gen.setAdd))
                    Toggle("Symbols !@#$%^&*", isOn: bindingBool($needShift18, setter: gen.setShift1_8))
                    Toggle("Braces (){}[]", isOn: bindingBool($needBrace, setter: gen.setBrace))
                    Toggle("Space", isOn: bindingBool($needSpace, setter: gen.setSpace))
                    Toggle("Question (?)", isOn: bindingBool($needQuestion, setter: gen.setQuestion))
                    Toggle("Slash /\\", isOn: bindingBool($needSlash, setter: gen.setSlash))
                    Toggle("Greater/Less <>", isOn: bindingBool($needGreaterLess, setter: gen.setGreaterLess))
                }

                Section(header: Text("Length")) {
                    Stepper(value: Binding(get: { length }, set: { newVal in
                        length = clampLen(newVal)
                        gen.setLength(Int32(length))
                        regenerate()
                    }), in: 4...128) {
                        HStack {
                            Text("Length")
                            Spacer()
                            Text("\(length)")
                        }
                    }
                    HStack {
                        Text("Custom")
                        TextField("Length", value: $length, format: .number)
                            .keyboardType(.numberPad)
                            .onChange(of: length) { _, newVal in
                                let v = clampLen(newVal)
                                if v != newVal { length = v }
                                gen.setLength(Int32(length))
                                regenerate()
                            }
                    }
                }

                Section(header: Text("Password")) {
                    HStack {
                        if showPassword {
                            TextField("Password", text: $password)
                                .textSelection(.enabled)
                        } else {
                            SecureField("Password", text: $password)
                        }
                        Button(action: { regenerate() }) { Image(systemName: "arrow.clockwise") }
                        Button(action: { UIPasteboard.general.string = password }) { Image(systemName: "doc.on.doc") }
                    }
                    Toggle("Show password", isOn: $showPassword)
                }
            }

            Divider()

            HStack {
                Button(role: .cancel) { onCancel() } label: { Text("Cancel").frame(maxWidth: .infinity) }
                    .buttonStyle(.bordered)
                Button(action: { onUse(password) }) { Text("Use Password").frame(maxWidth: .infinity) }
                    .buttonStyle(.borderedProminent)
                    .disabled(password.isEmpty)
            }
            .padding()
        }
        .navigationTitle("Password Generator")
        .onAppear { regenerate() }
    }

    private func clampLen(_ v: Int) -> Int { max(4, min(128, v)) }

    private func regenerate() {
        // push state into generator and pull new password
        gen.setLower(needLower)
        gen.setUpper(needUpper)
        gen.setDigit(needDigit)
        gen.setMinus(needMinus)
        gen.setAdd(needAdd)
        gen.setShift1_8(needShift18)
        gen.setBrace(needBrace)
        gen.setSpace(needSpace)
        gen.setQuestion(needQuestion)
        gen.setSlash(needSlash)
        gen.setGreaterLess(needGreaterLess)
        gen.setLength(Int32(length))
        password = gen.generate()
    }

    // Create a binding that also triggers a setter and regeneration
    private func bindingBool(_ state: Binding<Bool>, setter: @escaping (Bool) -> Void) -> Binding<Bool> {
        Binding<Bool>(get: { state.wrappedValue }, set: { newVal in
            state.wrappedValue = newVal
            setter(newVal)
            regenerate()
        })
    }
}

#Preview {
    NavigationView {
        PwdGeneratorView(onUse: { _ in }, onCancel: {})
    }
}
