#
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html.
# Run `pod lib lint face_sdk_3divi.podspec` to validate before publishing.
#
Pod::Spec.new do |s|
  s.name             = 'face_sdk_3divi'
  s.version          = '1.0.0'
  s.summary          = 'Face SDK plugin.'
  s.description      = <<-DESC
A new Flutter project.
                       DESC
  s.homepage         = 'https://face.3divi.com/products/face_sdk_3divi'
  s.license          = { :file => '../LICENSE' }
  s.author           = { '' => '' }
  s.source           = { :path => '.' }
  s.source_files = 'Classes/**/*'
  s.dependency 'Flutter'
  s.platform = :ios, '10.0'

  # Flutter.framework does not contain a i386 slice.
  s.pod_target_xcconfig = { 'DEFINES_MODULE' => 'YES', 'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'i386' }
  s.swift_version = '5.0'
end
